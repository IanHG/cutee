#pragma once
#ifndef CUTEE_MACROS_HPP_INCLUDED
#define CUTEE_MACROS_HPP_INCLUDED

#include "suite.hpp"
#include "float_eq.hpp"

/**
 * Assertion Macros
 **/
#define UNIT_ASSERT(a, b) \
   cutee::asserter::assertt(a, cutee::info{b, __FILE__, __LINE__});

#define UNIT_ASSERT_NOT(a, b) \
   cutee::asserter::assert_not(a, cutee::info{b, __FILE__, __LINE__});

#define UNIT_ASSERT_EQUAL(a, b, c) \
   cutee::asserter::assert_equal(a, b, cutee::info{c, __FILE__, __LINE__});

#define UNIT_ASSERT_NOT_EQUAL(a, b, c) \
   cutee::asserter::assert_not_equal(a, b, cutee::info{c, __FILE__, __LINE__});

#define UNIT_ASSERT_FEQUAL(a, b, c) \
   cutee::asserter::assert_float_equal_prec(a, b, 2u, cutee::info{c, __FILE__, __LINE__});

#define UNIT_ASSERT_FEQUAL_PREC(a, b, c, d) \
   cutee::asserter::assert_float_equal_prec(a, b, c, cutee::info{d, __FILE__, __LINE__});

#define UNIT_ASSERT_FZERO(a,b,c) \
   cutee::asserter::assert_float_numeq_zero_prec(a, b, 2u, cutee::info{c, __FILE__, __LINE__});

#define UNIT_ASSERT_FZERO_PREC(a,b,c,d) \
   cutee::asserter::assert_float_numeq_zero_prec(a, b, c, cutee::info{d, __FILE__, __LINE__});

#endif /* CUTEE_MACROS_HPP_INCLUDED */
