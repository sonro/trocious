#pragma once

#include "troc.lib.h"

#define TROC_test(suite_name, test_name) __TROC_test(suite_name, test_name)

#define TROC_expect(expr) __TROC_expect(expr)

#define TROC_assert(expr) __TROC_assert(expr)

#define TROC_failMsg(reason) __TROC_customFail(reason)

void TROC_runTests();

int TROC_finish();
