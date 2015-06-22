#ifndef GD_NETCDF_GLOBAL_H
#define GD_NETCDF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_NETCDF_LIBRARY)
#  define GD_NETCDF_EXPORT Q_DECL_EXPORT
#else
#  define GD_NETCDF_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_NETCDF_GLOBAL_H
