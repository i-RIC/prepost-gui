#ifndef GUI_API_H
#define GUI_API_H

#include <QtCore/qglobal.h>

#if defined(GUI_LIBRARY)
#  define GUI_API Q_DECL_EXPORT
#else
#  define GUI_API Q_DECL_IMPORT
#endif

#endif // GUI_API_H
