#ifndef POSTBASE_GLOBAL_H
#define POSTBASE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(POSTBASE_LIBRARY)
#  define POSTBASEDLL_EXPORT Q_DECL_EXPORT
#else
#  define POSTBASEDLL_EXPORT Q_DECL_IMPORT
#endif

#endif // POSTBASE_GLOBAL_H
