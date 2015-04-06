#ifndef GUIBASE_GLOBAL_H
#define GUIBASE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GUIBASE_LIBRARY)
#  define GUIBASEDLL_EXPORT Q_DECL_EXPORT
#else
#  define GUIBASEDLL_EXPORT Q_DECL_IMPORT
#endif

#endif // GUIBASE_GLOBAL_H
