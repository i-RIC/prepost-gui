#ifndef GD_POLYGON_GLOBAL_H
#define GD_POLYGON_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_POLYGON_LIBRARY)
#  define GD_POLYGON_EXPORT Q_DECL_EXPORT
#else
#  define GD_POLYGON_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_POLYGON_GLOBAL_H
