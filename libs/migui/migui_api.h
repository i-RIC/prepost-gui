#ifndef MIGUI_API_H
#define MIGUI_API_H

#include <QtCore/qglobal.h>

#if defined(MIGUI_LIBRARY)
#  define MIGUI_API Q_DECL_EXPORT
#else
#  define MIGUI_API Q_DECL_IMPORT
#endif

#endif // MIGUI_API_H
