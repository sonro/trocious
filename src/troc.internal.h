#pragma once

#include <stdlib.h>

typedef void (*TROC_TestFunction)();

typedef struct TROC_TestEntry {
    const char *suite_name;
    const char *test_name;
    TROC_TestFunction function;
} TROC_TestEntry;

void TROC_cleanup();

void TROC_registerTest(const char *suite_name, const char *test_name,
                       TROC_TestFunction test_func);

void TROC_deinitRegistry();

const TROC_TestEntry *TROC_getRegistry();

size_t TROC_getRegisteredCount();

void TROC_runTest(const TROC_TestEntry *entry);

[[noreturn]]
void TROC_fatalError(const char *msg, ...);
