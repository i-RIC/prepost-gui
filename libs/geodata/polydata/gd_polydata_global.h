#ifndef GD_POLYDATA_GLOBAL_H
#define GD_POLYDATA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_POLYDATA_LIBRARY)
#  define GD_POLYDATA_EXPORT Q_DECL_EXPORT
#else
#  define GD_POLYDATA_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_POLYDATA_GLOBAL_H
