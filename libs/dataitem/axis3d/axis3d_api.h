#ifndef AXIS3D_API_H
#define AXIS3D_API_H

#include <QtCore/QtGlobal>

#if defined(AXIS3D_LIBRARY)
#  define AXIS3D_API Q_DECL_EXPORT
#else
#  define AXIS3D_API Q_DECL_IMPORT
#endif

#endif // AXIS3D_API_H
