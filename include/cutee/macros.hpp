#include "suite.hpp"

/**
 * Assertion Macros
 **/
#define F_UNIT_ASSERT(a, b) \
   cutee::asserter::assertt(a, cutee::info{b, __FILE__, __LINE__});

#define F_UNIT_ASSERT_FEQUAL(a, b, c) \
   cutee::asserter::assert_float_equal_prec(a, b, 2u, cutee::info{c, __FILE__, __LINE__});

#define F_UNIT_ASSERT_EQUAL(a, b, c) \
   cutee::asserter::assert_equal(a, b, cutee::info{c, __FILE__, __LINE__});
