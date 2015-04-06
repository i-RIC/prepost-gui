#ifndef POST3D_GLOBAL_H
#define POST3D_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(POST3D_LIBRARY)
#  define POST3D_EXPORT Q_DECL_EXPORT
#else
#  define POST3D_EXPORT Q_DECL_IMPORT
#endif

#endif // POST3D_GLOBAL_H
