#ifndef MISC_GLOBAL_H
#define MISC_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(MISC_LIBRARY)
#  define MISCDLL_EXPORT Q_DECL_EXPORT
#else
#  define MISCDLL_EXPORT Q_DECL_IMPORT
#endif

#endif // MISC_GLOBAL_H
