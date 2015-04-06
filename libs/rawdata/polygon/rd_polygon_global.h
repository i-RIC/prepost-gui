#ifndef RD_POLYGON_GLOBAL_H
#define RD_POLYGON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RD_POLYGON_LIBRARY)
#  define RD_POLYGON_EXPORT Q_DECL_EXPORT
#else
#  define RD_POLYGON_EXPORT Q_DECL_IMPORT
#endif

#endif // RD_POLYGON_GLOBAL_H
