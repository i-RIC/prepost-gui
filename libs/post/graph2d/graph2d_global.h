#ifndef GRAPH2D_GLOBAL_H
#define GRAPH2D_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRAPH2D_LIBRARY)
#  define GRAPH2D_EXPORT Q_DECL_EXPORT
#else
#  define GRAPH2D_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPH2D_GLOBAL_H
