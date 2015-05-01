#ifndef GRAPHICSSUPPORT_H
#define GRAPHICSSUPPORT_H

#include <cmath>
#include <QTransform>
class QTransform;

namespace iRIC
{
	qreal getScale(const QTransform& t);
}
#endif // GRAPHICSSUPPORT_H
