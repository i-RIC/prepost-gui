#ifndef GEODATAPOLYLINE_COORDINATESEDITOR_H
#define GEODATAPOLYLINE_COORDINATESEDITOR_H

#include "../geodatapolyline.h"

class GeoDataPolyLine::CoordinatesEditor
{
public:
	static void edit(GeoDataPolyLine* polyline);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static GeoDataPolyLine* targetPolyLine;
	static bool applied;
};

#endif // GEODATAPOLYLINE_COORDINATESEDITOR_H
