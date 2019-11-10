#include "polylinecontroller.h"
#include "polylinecontrollerutil.h"

#include <geoio/coordinatesedit.h>

#include <QDataStream>
#include <QPointF>

void PolyLineControllerUtil::applyOffset(PolyLineController* polyLine, double x, double y)
{
	auto newLine = CoordinatesEdit::applyOffset(polyLine->polyLine(), - QPointF(x, y));
	polyLine->setPolyLine(newLine);
}

void PolyLineControllerUtil::loadPolyLine(QDataStream* stream, PolyLineController* polyLine, QPointF offset)
{
	std::vector<QPointF> line;
	int size;
	*stream >> size;
	for (int i = 0; i < size; ++i) {
		qreal x, y;
		*stream >> x >> y;
		line.push_back(QPointF(x - offset.x(), y - offset.y()));
	}
	polyLine->setPolyLine(line);
}

void PolyLineControllerUtil::savePolyLine(QDataStream* stream, const PolyLineController& polyLine, QPointF offset)
{
	auto line = polyLine.polyLine();
	int size = static_cast<int>(line.size());
	*stream << size;
	for (QPointF& p : line) {
		*stream << p.x() + offset.x() << p.y() + offset.y();
	}
}

PolyLineControllerUtil::PolyLineControllerUtil()
{}
