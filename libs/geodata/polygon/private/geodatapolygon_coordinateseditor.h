#ifndef GEODATAPOLYGON_COORDINATESEDITOR_H
#define GEODATAPOLYGON_COORDINATESEDITOR_H

#include "../geodatapolygon.h"

class GeoDataPolygon::CoordinatesEditor
{
public:
	static void edit(GeoDataPolygon* polygon);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static GeoDataPolygon* targetPolygon;
	static bool applied;
};

#endif // GEODATAPOLYGON_COORDINATESEDITOR_H
