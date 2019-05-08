#include "include/cutee/unit_assert.hpp"

namespace cutee
{

detail::file_info make_file_info(const char* file, const unsigned int line)
{
   return detail::file_info(file, line);
}

} /* namespace cutee */
