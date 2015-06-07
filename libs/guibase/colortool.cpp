#include "colortool.h"

#include <QColor>

namespace iRIC
{

	void QColorToVTKColor(const QColor& from, double to[3])
	{
		to[0] = from.redF();
		to[1] = from.greenF();
		to[2] = from.blueF();
	}

	void VTKColorToQColor(double from[3], QColor& to)
	{
		to.setRedF(from[0]);
		to.setGreenF(from[1]);
		to.setBlueF(from[2]);
	}
}
