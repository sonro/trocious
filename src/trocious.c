#include <stdarg.h>
#include <stdio.h>

#include "troc.internal.h"
#include "trocious.h"

static thread_local const TROC_TestEntry *TROC_current_test = NULL;

static size_t tests_run = 0;

void TROC_runTest(const TROC_TestEntry *entry) {
    TROC_current_test = entry;
    tests_run++;
    (*entry->function)();
}

void TROC_runTests() {
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

int TROC_finish() {
    TROC_cleanup();
    return EXIT_SUCCESS;
}

void __TROC_registerTest(const char *suite_name, const char *test_name,
                         TROC_TestFunction test_func) {
    TROC_registerTest(suite_name, test_name, test_func);
}

void TROC_cleanup() {
    TROC_deinitRegistry();
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
