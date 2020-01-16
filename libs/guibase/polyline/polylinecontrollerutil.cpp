#include "polylinecontroller.h"
#include "polylinecontrollerutil.h"
#include "polylineutil.h"

#include "../coordinates/coordinatesedit.h"

#include <vtkPolyData.h>

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

std::vector<QPointF> PolyLineControllerUtil::buildSplinePoints(const PolyLineController& controller, int factor)
{
	const auto line = controller.polyLine();
	if (line.size() < 2) {
		std::vector<QPointF> empty;
		return empty;
	}
	return PolyLineUtil::buildSplinePoints(controller.polyData()->GetPoints(), static_cast<int>(line.size()) * factor);
}

PolyLineControllerUtil::PolyLineControllerUtil()
{}
