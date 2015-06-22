#ifndef GD_POINTMAP_GLOBAL_H
#define GD_POINTMAP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GD_POINTMAP_LIBRARY)
#  define GD_POINTMAP_EXPORT Q_DECL_EXPORT
#else
#  define GD_POINTMAP_EXPORT Q_DECL_IMPORT
#endif

#endif // GD_POINTMAP_GLOBAL_H
