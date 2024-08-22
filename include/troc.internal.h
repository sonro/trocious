#pragma once

#include <stdlib.h>

#define TROC_init_capacity 16
#define TROC_fail_buf_size 1024

typedef void (*TROC_TestFunction)();

typedef struct TROC_TestEntry {
    const char *suite_name;
    const char *test_name;
    TROC_TestFunction function;
} TROC_TestEntry;

typedef struct TROC_Failure {
    const char *expr;
    const char *file;
    const int line;
} TROC_Failure;

void TROC_init();

void TROC_registerTest(const char *suite_name, const char *test_name,
                       TROC_TestFunction test_func);

void TROC_deinitRegistry();

const TROC_TestEntry *TROC_getRegistry();

size_t TROC_getRegisteredCount();

void TROC_runTest(const TROC_TestEntry *entry);

void TROC_printSummary();

void TROC_cleanup();

void TROC_success();

void TROC_fail(const TROC_Failure failure);

[[noreturn]]
void TROC_failExit(const TROC_Failure failure);

[[noreturn]]
void TROC_fatalError(const char *msg, ...);

void TROC_ensureCapacity(void **ptr, size_t *existing, size_t new,
                         size_t elem_size);

#define TROC_Failure_new(expr_str)                                             \
    (TROC_Failure) {                                                           \
        .expr = expr_str, .file = __FILE__, .line = __LINE__                   \
    }

#define __TROC_test(suite_name, test_name)                                     \
    void test_##suite_name##_##test_name(void);                                \
    __attribute__((                                                            \
        constructor)) static void register_##suite_name##_##test_name(void) {  \
        TROC_registerTest(#suite_name, #test_name,                             \
                          test_##suite_name##_##test_name);                    \
    }                                                                          \
    void test_##suite_name##_##test_name(void)

#define __TROC_assert(expr)                                                    \
    if (expr) {                                                                \
        TROC_success();                                                        \
    } else {                                                                   \
        const TROC_Failure __failure = TROC_Failure_new(#expr);                \
        TROC_failExit(__failure);                                              \
    }

#define __TROC_expect(expr)                                                    \
    if (expr) {                                                                \
        TROC_success();                                                        \
    } else {                                                                   \
        const TROC_Failure __failure = TROC_Failure_new(#expr);                \
        TROC_fail(__failure);                                                  \
    }
