#ifndef COLORTOOL_H
#define COLORTOOL_H

#include "guibase_global.h"

class QColor;

namespace iRIC {
	void GUIBASEDLL_EXPORT QColorToVTKColor(const QColor &from, double to[3]);
	void GUIBASEDLL_EXPORT VTKColorToQColor(double from[3], QColor & to);
}

#endif // COLORTOOL_H
