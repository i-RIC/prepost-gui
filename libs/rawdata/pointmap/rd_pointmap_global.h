#ifndef RD_POINTMAP_GLOBAL_H
#define RD_POINTMAP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RD_POINTMAP_LIBRARY)
#  define RD_POINTMAP_EXPORT Q_DECL_EXPORT
#else
#  define RD_POINTMAP_EXPORT Q_DECL_IMPORT
#endif

#endif // RD_POINTMAP_GLOBAL_H
