#pragma once

#define TROC_init_capacity 16
#define TROC_fail_buf_size 1024

typedef void (*_TROC_TestFunction)();

typedef struct _TROC_Failure {
    const char *expr;
    const char *file;
    const int line;
} _TROC_Failure;

void _TROC_registerTest(const char *suite_name, const char *test_name,
                        _TROC_TestFunction test_func);

void _TROC_success();

void _TROC_fail(const _TROC_Failure failure);

[[noreturn]]
void _TROC_failExit(const _TROC_Failure failure);

#define __TROC_Failure_new(expr_str)                                           \
    (_TROC_Failure) {                                                          \
        .expr = expr_str, .file = __FILE__, .line = __LINE__                   \
    }

#define __TROC_test(suite_name, test_name)                                     \
    void test_##suite_name##_##test_name(void);                                \
    __attribute__((                                                            \
        constructor)) static void register_##suite_name##_##test_name(void) {  \
        _TROC_registerTest(#suite_name, #test_name,                            \
                           test_##suite_name##_##test_name);                   \
    }                                                                          \
    void test_##suite_name##_##test_name(void)

#define __TROC_assert(expr)                                                    \
    if (expr) {                                                                \
        _TROC_success();                                                       \
    } else {                                                                   \
        const _TROC_Failure __failure = __TROC_Failure_new(#expr);             \
        _TROC_failExit(__failure);                                             \
    }

#define __TROC_expect(expr)                                                    \
    if (expr) {                                                                \
        _TROC_success();                                                       \
    } else {                                                                   \
        const _TROC_Failure __failure = __TROC_Failure_new(#expr);             \
        _TROC_fail(__failure);                                                 \
    }

#define __TROC_customFail(reason)                                              \
    const _TROC_Failure __failure = __TROC_Failure_new(reason);                \
    TROC_fail(__failure)
