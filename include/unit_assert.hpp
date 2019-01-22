#ifndef CUTEE_UNIT_ASSERT_H_INCLUDED
#define CUTEE_UNIT_ASSERT_H_INCLUDED

#include <string>
#include "float_eq.hpp"

namespace cutee
{

namespace detail
{

//
// data structure for holding data to assert including user defined message
//
template<class A, class E>
struct unit_assert_data
{
   template<class AA, class EE>
   unit_assert_data(AA&& actual, EE&& expected, const std::string& message):
      _actual(std::forward<AA>(actual)), _expected(std::forward<EE>(expected)), _message(message)
   {
   }

   const A& _actual;
   const E& _expected;
   const std::string& _message;
};

//
// data structure to information about a location in a file
//
struct file_info
{
   file_info(const char* file, const unsigned int line): _file(file), _line(line)
   {
   }

   const std::string  _file;
   const unsigned int _line;
};

} // namespace detail

//
// construction wrapper
//
detail::file_info make_file_info(const char* file, const unsigned int line);

//
// construction wrapper
//
template<class A, class E>
detail::unit_assert_data<A,E> make_unit_assert_data(A&& actual, E&& expected, const std::string& message)
{
   return detail::unit_assert_data<A,E>(std::forward<A>(actual),std::forward<E>(expected),message);
}

//
//
//
template<class A, class E>
inline void unit_assert(std::string name, detail::unit_assert_data<A,E>&& data, detail::file_info&& finfo)
{
   if(!data._actual)
      throw test_failed(name,finfo._file.c_str(),finfo._line,data._message,data._expected,data._actual);
}

//
// wrapper function for asserting equal
//
template<class A, class E>
inline void unit_assert_equal(std::string name, detail::unit_assert_data<A,E>&& data, detail::file_info&& finfo)
{
   if(data._actual != data._expected)
      throw test_failed(name,finfo._file.c_str(),finfo._line,data._message,data._expected,data._actual);
}

//
// wrapper function for asserting not equal
//
template<class A, class E>
inline void unit_assert_not_equal(std::string name, detail::unit_assert_data<A,E>&& data, detail::file_info&& finfo)
{
   if(data._actual == data._expected)
      throw test_failed(name,finfo._file.c_str(),finfo._line,data._message,data._expected,data._actual);
}

//
// wrapper function for asserting float equal
//
template<class A, class E>
inline void unit_assert_fequal(std::string name, detail::unit_assert_data<A,E>&& data, detail::file_info&& finfo)
{
   if(!numeric::float_eq(data._actual,data._expected))
      throw test_failed(name,finfo._file.c_str(),finfo._line,data._message,data._expected,data._actual);
}

//
// undef if macro is set
//
#ifdef UNDEF_ASSERT_COMMANDS
#undef ASSERT
#undef UNIT_ASSERT
#undef UNIT_ASSERT_OLD
#undef UNIT_ASSERT_EQUAL
#undef UNIT_ASSERT_EQUAL_OLD
#undef UNIT_ASSERT_NOT_EQUAL
#undef UNIT_ASSERT_FEQUAL
#undef UNIT_ASSERT_FEQUAL_OLD
#undef UNIT_ASSERT_FEQUAL_PREC
#endif /* UNDEF_ASSERT_COMMANDS */

//
// ASSERTION MACROS
//
// do assertion (will not increase assertion counter)
//
#define ASSERT(a,b) \
   if(!(a)) throw test_failed("no name",__FILE__,__LINE__,(b),true,(a))

//
// do assertion and increase assertion counter (new type using wrappers, for only one evaluation of the arguments (a,b,c))
//
#define UNIT_ASSERT(a,b) \
   cutee::test::incr_num_assertions(); \
   cutee::unit_assert(this->name(),cutee::make_unit_assert_data((a),true,(b)),cutee::make_file_info(__FILE__,__LINE__));

//
// do assertion and increase assertion counter
//
#define UNIT_ASSERT_OLD(a,b) \
   cutee::test::incr_num_assertions(); \
   if(!(a)) throw test_failed(this->name(),__FILE__,__LINE__,(b),true,(a))

//
// do assertion and increase assertion counter
//
#define UNIT_ASSERT_EQUAL(a,b,c) \
   cutee::test::incr_num_assertions(); \
   cutee::unit_assert_equal(this->name(),cutee::make_unit_assert_data((a),(b),(c)),cutee::make_file_info(__FILE__,__LINE__));

#define UNIT_ASSERT_EQUAL_OLD(a,b,c) \
   cutee::test::incr_num_assertions(); \
   if(!((b)==(a))) throw test_failed(this->name(),__FILE__,__LINE__,(c),(b),(a))

//
// do assertion and increase assertion counter
//
#define UNIT_ASSERT_NOT_EQUAL(a,b,c) \
   cutee::test::incr_num_assertions(); \
   cutee::unit_assert_not_equal(this->name(),cutee::make_unit_assert_data((a),(b),(c)),cutee::make_file_info(__FILE__,__LINE__));

//
// do assertion and increase assertion counter
//
#define UNIT_ASSERT_FEQUAL(a,b,c) \
   cutee::test::incr_num_assertions(); \
   cutee::unit_assert_fequal(this->name(),cutee::make_unit_assert_data((a),(b),(c)),cutee::make_file_info(__FILE__,__LINE__));

#define UNIT_ASSERT_FEQUAL_OLD(a,b,c) \
   cutee::test::incr_num_assertions(); \
   if(!cutee::numeric::float_eq((a),(b))) \
      throw cutee::test_failed(this->name(),__FILE__,__LINE__,(c),(b),(a))

//
// do assertion and increase assertion counter
//
#define UNIT_ASSERT_FEQUAL_PREC(a,b,c,d) \
   cutee::test::incr_num_assertions(); \
   if(!cutee::numeric::float_eq((a),(b),(c))) \
      throw cutee::test_failed(this->name(),__FILE__,__LINE__,(d),(b),(a))

} /* namespace cutee */

#endif /* CUTEE_UNIT_ASSERT_H_INCLUDED */
