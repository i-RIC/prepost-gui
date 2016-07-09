#ifndef GEOIO_GLOBAL_H
#define GEOIO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GEOIO_LIBRARY)
#  define GEOIODLL_EXPORT Q_DECL_EXPORT
#else
#  define GEOIODLL_EXPORT Q_DECL_IMPORT
#endif

#endif // GEOIO_GLOBAL_H
