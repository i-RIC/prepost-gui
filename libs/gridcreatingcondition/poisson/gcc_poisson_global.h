#ifndef GCC_POISSON_GLOBAL_H
#define GCC_POISSON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCC_POISSON_LIBRARY)
#  define GCC_POISSON_EXPORT Q_DECL_EXPORT
#else
#  define GCC_POISSON_EXPORT Q_DECL_IMPORT
#endif

#endif // GCC_POISSON_GLOBAL_H
