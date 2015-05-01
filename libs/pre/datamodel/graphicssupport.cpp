#include <cmath>
#include <QTransform>

namespace iRIC
{
	qreal getScale(const QTransform& t)
	{
		return std::sqrt(t.m11() * t.m22() - t.m12() * t.m21());
	}
}

