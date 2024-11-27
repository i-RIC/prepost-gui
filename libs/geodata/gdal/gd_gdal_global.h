#ifndef GD_GDAL_GLOBAL_H
#define GD_GDAL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_GDAL_LIBRARY)
#  define GD_GDAL_EXPORT Q_DECL_EXPORT
#else
#  define GD_GDAL_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_GDAL_GLOBAL_H
