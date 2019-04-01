#ifndef GCC_LAPLACE_GLOBAL_H
#define GCC_LAPLACE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GCC_LAPLACE_LIBRARY)
#  define GCC_LAPLACE_EXPORT Q_DECL_EXPORT
#else
#  define GCC_LAPLACE_EXPORT Q_DECL_IMPORT
#endif

#endif // GCC_LAPLACE_GLOBAL_H
