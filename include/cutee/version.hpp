#pragma once
#ifndef CUTEE_VERSION_HPP_INCLUDED
#define CUTEE_VERSION_HPP_INCLUDED

#undef CUTEE_VERION_MAJOR
#undef CUTEE_VERION_MINOR
#undef CUTEE_VERION_PATCH
#undef CUTEE_VERION

#define CUTEE_VERSION_MAJOR 2
#define CUTEE_VERSION_MINOR 0
#define CUTEE_VERSION_PATCH 3

#define CUTEE_VERSION CUTEE_VERSION_MAJOR.CUTEE_VERSION_MINOR.CUTEE_VERSION_PATCH

#define XSTRINGIFY(a) STRINGIFY(a)
#define STRINGIFY(a) #a

namespace cutee
{
   constexpr const char* const version()
   {
      return XSTRINGIFY(CUTEE_VERSION);
   }

   constexpr int version_major()
   {
      return CUTEE_VERSION_MAJOR;
   }
   
   constexpr int version_minor()
   {
      return CUTEE_VERSION_MINOR;
   }
   
   constexpr int version_patch()
   {
      return CUTEE_VERSION_PATCH;
   }
}

#endif /* CUTEE_VERSION_HPP_INCLUDED */
