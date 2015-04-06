#ifndef POST2D_GLOBAL_H
#define POST2D_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(POST2D_LIBRARY)
#  define POST2D_EXPORT Q_DECL_EXPORT
#else
#  define POST2D_EXPORT Q_DECL_IMPORT
#endif

#endif // POST2D_GLOBAL_H
