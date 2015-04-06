#ifndef PRE_GLOBAL_H
#define PRE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PRE_LIBRARY)
#  define PREDLL_EXPORT Q_DECL_EXPORT
#else
#  define PREDLL_EXPORT Q_DECL_IMPORT
#endif

#endif // PRE_GLOBAL_H
