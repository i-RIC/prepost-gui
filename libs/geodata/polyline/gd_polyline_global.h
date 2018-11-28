#ifndef GD_POLYLINE_GLOBAL_H
#define GD_POLYLINE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_POLYLINE_LIBRARY)
#  define GD_POLYLINE_EXPORT Q_DECL_EXPORT
#else
#  define GD_POLYLINE_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_POLYLINE_GLOBAL_H
