#ifndef GRAPHICSSUPPORT_H
#define GRAPHICSSUPPORT_H

#include <cmath>
#include <QTransform>
class QTransform;

namespace iRIC
{
	qreal getScale(const QTransform& t) override;
}
#endif // GRAPHICSSUPPORT_H
