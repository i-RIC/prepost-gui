#ifndef GD_POINT_GLOBAL_H
#define GD_POINT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_POINT_LIBRARY)
#  define GD_POINT_EXPORT Q_DECL_EXPORT
#else
#  define GD_POINT_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_POINT_GLOBAL_H
