#ifndef RD_NETCDF_GLOBAL_H
#define RD_NETCDF_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RD_NETCDF_LIBRARY)
#  define RD_NETCDF_EXPORT Q_DECL_EXPORT
#else
#  define RD_NETCDF_EXPORT Q_DECL_IMPORT
#endif

#endif // RD_NETCDF_GLOBAL_H
