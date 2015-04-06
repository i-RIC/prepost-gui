#ifndef GCC_TRIANGLE_GLOBAL_H
#define GCC_TRIANGLE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCC_TRIANGLE_LIBRARY)
#  define GCC_TRIANGLE_EXPORT Q_DECL_EXPORT
#else
#  define GCC_TRIANGLE_EXPORT Q_DECL_IMPORT
#endif

#endif // GCC_TRIANGLE_GLOBAL_H
