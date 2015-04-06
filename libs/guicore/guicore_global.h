#ifndef GUICORE_GLOBAL_H
#define GUICORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GUICORE_LIBRARY)
#  define GUICOREDLL_EXPORT Q_DECL_EXPORT
#else
#  define GUICOREDLL_EXPORT Q_DECL_IMPORT
#endif

#endif // GUICORE_GLOBAL_H
