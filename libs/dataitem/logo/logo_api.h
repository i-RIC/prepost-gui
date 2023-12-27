#ifndef LOGO_API_H
#define LOGO_API_H

#include <QtCore/QtGlobal>

#if defined(LOGO_LIBRARY)
#  define LOGO_API Q_DECL_EXPORT
#else
#  define LOGO_API Q_DECL_IMPORT
#endif

#endif // LOGO_API_H
