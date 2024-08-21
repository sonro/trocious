#pragma once

void TROC_runTests();

int TROC_finish();

void __TROC_registerTest(const char *suite_name, const char *test_name,
                         void (*test_func)());

#define TROC_test(suite_name, test_name)                                       \
    void test_##suite_name##_##test_name(void);                                \
    __attribute__((                                                            \
        constructor)) static void register_##suite_name##_##test_name(void) {  \
        __TROC_registerTest(#suite_name, #test_name,                           \
                            test_##suite_name##_##test_name);                  \
    }                                                                          \
    void test_##suite_name##_##test_name(void)
