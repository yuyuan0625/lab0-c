#ifndef DUDECT_FIXTURE_H
#define DUDECT_FIXTURE_H
#define DUDECT_NUMBER_PERCENTILES (100)

#include <stdbool.h>
#include "constant.h"

/* Interface to test if function is constant */
#define _(x) bool is_##x##_const(void);
DUT_FUNCS
#undef _

#endif
