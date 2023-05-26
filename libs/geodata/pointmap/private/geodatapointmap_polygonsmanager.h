#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_H

#include "../geodatapointmap.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPolygonGroup;

class GeoDataPointmap::PolygonsManager : public QObject, public MouseEventController
{
public:
	PolygonsManager(PointsManager* pm, TINManager* tm, GeoDataPointmap* pointmap);
	~PolygonsManager();

	void updatePolygonsActorSettings() const;
	void loadPolygons(const QString& filename);
	void savePolygons(const QString& filename);

	void updatePolygonOrder();
	bool map(double x, double y, double* value);
	GeoDataPolygonGroup* polygonGroup() const;

	void addActionsToMenu(QMenu* menu) override;

	void activate(VTK2DGraphicsView* v) override;
	void deactivate(VTK2DGraphicsView* v) override;

	void handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;
	void handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v) override;

private:
	class AttributeForMappingCondition;
	AttributeForMappingCondition* m_attribute;

	GeoDataPolygonGroup* m_polygonGroup;
	PointsManager* m_pointsManager;
	TINManager* m_tinManager;
	GeoDataPointmap* m_parent;

	class MappingSetting;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_H
