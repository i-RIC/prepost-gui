#ifndef CS_API_H
#define CS_API_H

#include <QtCore/QtGlobal>

#if defined(CS_LIBRARY)
#  define CS_API Q_DECL_EXPORT
#else
#  define CS_API Q_DECL_IMPORT
#endif

#endif // CS_API_H
