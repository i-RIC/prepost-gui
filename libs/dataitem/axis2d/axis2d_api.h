#ifndef AXIS2D_API_H
#define AXIS2D_API_H

#include <QtCore/QtGlobal>

#if defined(AXIS2D_LIBRARY)
#  define AXIS2D_API Q_DECL_EXPORT
#else
#  define AXIS2D_API Q_DECL_IMPORT
#endif

#endif // AXIS2D_API_H
