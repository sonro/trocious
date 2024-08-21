#include <assert.h>
#include <trocious.h>

int main() {
    TROC_runTests();
    return TROC_finish();
}

TROC_test(foo, bar) {
    assert(2 == 2);
}
