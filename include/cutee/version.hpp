#pragma once
#ifndef CUTEE_VERSION_HPP_INCLUDED
#define CUTEE_VERSION_HPP_INCLUDED

#define CUTEE_VERSION_MAJOR 2
#define CUTEE_VERSION_MINOR 0
#define CUTEE_VERSION_PATCH 1

#define CUTEE_VERSION CUTEE_VERSION_MAJOR.CUTEE_VERSION_MINOR.CUTEE_VERSION_PATCH

#define XSTRINGIFY(a) STRINGIFY(a)
#define STRINGIFY(a) #a

namespace cutee
{
   constexpr const char* const version()
   {
      return XSTRINGIFY(CUTEE_VERSION);
   }
}

#endif /* CUTEE_VERSION_HPP_INCLUDED */
