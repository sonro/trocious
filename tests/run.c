#include <trocious.h>

int main() {
    TROC_runTests();
    return TROC_finish();
}

TROC_test(foo, bar) {
    TROC_expect(2 == 2);
}
