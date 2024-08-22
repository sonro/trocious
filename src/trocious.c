#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "troc.internal.h"
#include "trocious.h"

#define PRINT_TURQUOISE "\033[0;36m"
#define PRINT_RED "\033[0;31m"
#define PRINT_GREEN "\033[0;32m"
#define PRINT_RESET "\033[0m"

static thread_local const TROC_TestEntry *TROC_current_test = NULL;

static size_t tests_run = 0;
static size_t tests_failed = 0;
static size_t fail_strs_count = 0;
static const char **fail_strs = NULL;
static size_t fail_strs_capacity = 0;
static bool stdout_is_tty = false;

static void failStrAppend(const char *str);
static void deinitFailStrs();
static char *Failure_format(const TROC_Failure failure);
static void ensureFailStrCapacity(size_t capacity);

void TROC_runTests() {
    TROC_init();
    const TROC_TestEntry *test_entries = TROC_getRegistry();
    if (test_entries == NULL) {
        fprintf(stderr, "No tests to run\n");
        TROC_cleanup();
        exit(EXIT_SUCCESS);
    }
    const size_t test_count = TROC_getRegisteredCount();
    for (size_t i = 0; i < test_count; i++) {
        TROC_runTest(&test_entries[i]);
    }
}

void TROC_init() {
    stdout_is_tty = isatty(STDOUT_FILENO);
    if (stdout_is_tty) {
        printf("%sTROCIOUS%s testing:\n\n", PRINT_TURQUOISE, PRINT_RESET);
    } else {
        printf("TROCIOUS Running tests:\n\n");
    }
    tests_run = 0;
    tests_failed = 0;
    fail_strs_count = 0;
    fail_strs = NULL;
    fail_strs_capacity = 0;
}

void TROC_runTest(const TROC_TestEntry *entry) {
    size_t current_fails = fail_strs_count;
    TROC_current_test = entry;
    tests_run++;
    (*entry->function)();
    if (fail_strs_count > current_fails) {
        tests_failed++;
    }
}

int TROC_finish() {
    TROC_printSummary();
    TROC_cleanup();
    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

void TROC_cleanup() {
    TROC_deinitRegistry();
    deinitFailStrs();
}

void TROC_printSummary() {
    printf("\n\n");
    size_t tests_passed = tests_run - tests_failed;

    if (tests_failed > 0) {
        if (stdout_is_tty) {
            printf("%sFailures:%s\n", PRINT_TURQUOISE, PRINT_RESET);
        }
        for (size_t i = 0; i < fail_strs_count; i++) {
            printf("    %s\n", fail_strs[i]);
        }
    }

    if (stdout_is_tty) {
        printf("%sSummary:%s\n", PRINT_TURQUOISE, PRINT_RESET);
        printf("    Tests passed: %s%zu\n%s", PRINT_GREEN, tests_passed,
               PRINT_RESET);
        printf("    Tests failed: %s%zu\n%s",
               tests_failed > 0 ? PRINT_RED : PRINT_GREEN, tests_failed,
               PRINT_RESET);
    } else {
        printf("Summary:\n");
        printf("    Tests passed: %zu\n", tests_passed);
        printf("    Tests failed: %zu\n", tests_failed);
    }
}

void TROC_success() {
    putchar('.');
}

void TROC_fail(const TROC_Failure failure) {
    if (stdout_is_tty) {
        printf("%sF%s", PRINT_RED, PRINT_RESET);
    } else {
        putchar('F');
    }
    const char *fail_str = Failure_format(failure);
    failStrAppend(fail_str);
}

[[noreturn]]
void TROC_failExit(const TROC_Failure failure) {
    TROC_fail(failure);
    tests_failed++;
    printf("! Fatal Assertion");
    TROC_finish();
    exit(EXIT_FAILURE);
}

[[noreturn]]
void TROC_fatalError(const char *msg, ...) {
    fprintf(stderr, "A trocious fatal error: ");

    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");

    TROC_cleanup();
    exit(EXIT_FAILURE);
}

void TROC_ensureCapacity(void **ptr, size_t *existing, size_t new,
                         size_t elem_size) {
    size_t old_cap = *existing;
    if (old_cap >= new) {
        return;
    }
    size_t new_cap = (old_cap == 0) ? TROC_init_capacity : old_cap * 2;
    void *new_ptr = realloc(*ptr, new_cap * elem_size);
    if (new_ptr == NULL) {
        TROC_fatalError("Out of memory");
    }
    *ptr = new_ptr;
    *existing = new_cap;
}

static char *Failure_format(const TROC_Failure failure) {
    char buf[TROC_fail_buf_size] = {0};
    const char *filename = basename((char *)failure.file);
    const char *suite_name = TROC_current_test->suite_name;
    const char *test_name = TROC_current_test->test_name;
    if (stdout_is_tty) {
        snprintf(buf, TROC_fail_buf_size, "%s%s: %s%s\n    %s:%d %s\n",
                 PRINT_RED, suite_name, test_name, PRINT_RESET, filename,
                 failure.line, failure.expr);
    } else {
        snprintf(buf, TROC_fail_buf_size, "%s: %s\n    %s:%d %s\n", suite_name,
                 test_name, filename, failure.line, failure.expr);
    }

    size_t len = strlen(buf);
    if (len >= TROC_fail_buf_size) {
        TROC_fatalError("Failure message too long: '%s'", buf);
    }

    char *fail_str = calloc(len + 1, sizeof(char));
    if (fail_str == NULL) {
        TROC_fatalError("Out of memory");
    }

    memcpy(fail_str, buf, len);
    return fail_str;
}

static void failStrAppend(const char *str) {
    size_t new_count = fail_strs_count + 1;
    ensureFailStrCapacity(new_count);
    fail_strs[fail_strs_count] = str;
    fail_strs_count = new_count;
}

static void ensureFailStrCapacity(size_t capacity) {
    TROC_ensureCapacity((void **)&fail_strs, &fail_strs_capacity, capacity,
                        sizeof(char *));
}

static void deinitFailStrs() {
    if (fail_strs == NULL) {
        return;
    }
    for (size_t i = 0; i < fail_strs_count; i++) {
        if (fail_strs[i] != NULL) {
            free((void *)fail_strs[i]);
        }
    }
    free(fail_strs);
}
