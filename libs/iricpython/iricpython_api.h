#ifndef IRICPYTHON_API_H
#define IRICPYTHON_API_H

#include <QtCore/qglobal.h>

#if defined(IRICPYTHON_LIBRARY)
#  define IRICPYTHON_API Q_DECL_EXPORT
#else
#  define IRICPYTHON_API Q_DECL_IMPORT
#endif

#endif // IRICPYTHON_API_H
