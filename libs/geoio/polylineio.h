#ifndef POLYLINEIO_H
#define POLYLINEIO_H

#include "geoio_global.h"

#include <vector>

class QPointF;
class QWidget;

class GEOIODLL_EXPORT PolylineIO
{
public:
	static std::vector<QPointF> importData(QWidget* w);
	static void exportData(const std::vector<QPointF>& polyline, QWidget* w);

private:
	PolylineIO();
};

#endif // POLYLINEIO_H
