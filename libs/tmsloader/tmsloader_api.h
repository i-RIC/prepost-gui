#ifndef TMSLOADER_API_H
#define TMSLOADER_API_H

#include <QtCore/QtGlobal>

#if defined(TMSLOADER_LIBRARY)
#  define TMSLOADER_API Q_DECL_EXPORT
#else
#  define TMSLOADER_API Q_DECL_IMPORT
#endif

#endif // TMSLOADER_API_H
