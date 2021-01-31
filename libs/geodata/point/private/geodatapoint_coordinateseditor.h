#ifndef GEODATAPOINT_COORDINATESEDITOR_H
#define GEODATAPOINT_COORDINATESEDITOR_H

#include "../geodatapoint.h"

class GeoDataPoint::CoordinatesEditor
{
public:
	static void edit(GeoDataPoint* point);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static GeoDataPoint* targetPoint;
	static bool applied;
};

#endif // GEODATAPOINT_COORDINATESEDITOR_H
