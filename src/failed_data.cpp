#include "include/failed_data.hpp"

namespace cutee
{

/**
 *
 **/
void failed_data_destroy(failed_data_base* a_failed_data)
{ 
   delete a_failed_data; 
}

} /* namespace cutee */
