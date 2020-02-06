#ifndef POLYLINECONTROLLERUTIL_H
#define POLYLINECONTROLLERUTIL_H

#include "../guibase_global.h"

#include <QPointF>

#include <vector>

class PolyLineController;

class QDataStream;

class GUIBASEDLL_EXPORT PolyLineControllerUtil
{
public:
	static void applyOffset(PolyLineController* polyLine, double x, double y);
	static void loadPolyLine(QDataStream* stream, PolyLineController* polyLine, QPointF offset = QPointF(0, 0));
	static void savePolyLine(QDataStream* stream, const PolyLineController& polyLine, QPointF offset = QPointF(0, 0));
	static std::vector<QPointF> buildSplinePoints(const PolyLineController& line, int factor);

private:
	PolyLineControllerUtil();
};

#endif // POLYLINECONTROLLERUTIL_H
