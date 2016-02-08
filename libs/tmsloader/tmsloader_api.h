#ifndef TMSLOADER_API_H
#define TMSLOADER_API_H

#if defined(TMSLOADER_LIBRARY)
#  define TMSLOADER_API __declspec(dllexport)
#else
#  define TMSLOADER_API __declspec(dllimport)
#endif

#endif // TMSLOADER_API_H
