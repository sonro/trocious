#pragma once

#include <stdlib.h>

#include "troc.lib.h"

typedef struct _TROC_TestEntry {
    const char *suite_name;
    const char *test_name;
    _TROC_TestFunction function;
} TROC_TestEntry;

void TROC_init();

void TROC_deinitRegistry();

const TROC_TestEntry *TROC_getRegistry();

size_t TROC_getRegisteredCount();

void TROC_runTest(const TROC_TestEntry *entry);

void TROC_printSummary();

void TROC_cleanup();

[[noreturn]]
void TROC_fatalError(const char *msg, ...);

void TROC_ensureCapacity(void **ptr, size_t *existing, size_t new,
                         size_t elem_size);
