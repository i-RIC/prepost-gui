
#ifndef IRICLIBDLL
# ifdef _WIN32
#  if defined(IRICLIBDLL_LIBRARY)
#    define IRICLIBDLL _declspec(dllexport)
#  else
#    define IRICLIBDLL _declspec(dllimport)
#  endif
# else
#  define IRICLIBDLL
# endif
#endif

