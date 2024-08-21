#include "troc.internal.h"

#define init_capacity 16

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
    if (test_capacity >= capacity) {
        return;
    }
    size_t new_capacity =
        (test_capacity == 0) ? init_capacity : test_capacity * 2;
    TROC_TestEntry *new_entries =
        realloc(test_entries, sizeof(TROC_TestEntry) * new_capacity);
    if (new_entries == NULL) {
        TROC_fatalError("Out of memory");
    }
    test_entries = new_entries;
    test_capacity = new_capacity;
}
