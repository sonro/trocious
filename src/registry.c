#include "troc.internal.h"

static TROC_TestEntry *test_entries = NULL;
static size_t test_count = 0;
static size_t test_capacity = 0;

static void ensureCapacity(size_t capacity);

void TROC_registerTest(const char *suite_name, const char *test_name,
                       TROC_TestFunction test_func) {
    size_t new_count = test_count + 1;
    ensureCapacity(new_count);
    TROC_TestEntry entry = {suite_name, test_name, test_func};
    test_entries[test_count] = entry;
    test_count = new_count;
}

void TROC_deinitRegistry() {
    if (test_entries == NULL) {
        free(test_entries);
        test_entries = NULL;
        test_count = 0;
        test_capacity = 0;
    }
}

const TROC_TestEntry *TROC_getRegistry() {
    return test_entries;
}

size_t TROC_getRegisteredCount() {
    return test_count;
}

static void ensureCapacity(size_t capacity) {
    TROC_ensureCapacity((void **)&test_entries, &test_capacity, capacity,
                        sizeof(TROC_TestEntry));
}
