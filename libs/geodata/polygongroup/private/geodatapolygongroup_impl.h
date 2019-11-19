#ifndef GEODATAPOLYGONGROUP_IMPL_H
#define GEODATAPOLYGONGROUP_IMPL_H

#include"../geodatapolygongroup.h"
#include"../geodatapolygongrouppolygon.h"

#include <geodata/polygon/geodatapolygon.h>
#include <guibase/vtktool/vtkpolydatalinesactor.h>
#include <misc/zdepthrange.h>

#include <unordered_set>
#include <vector>

namespace geos {
namespace index{
namespace quadtree {
class Quadtree;
} // quadtree
} // index
} // geos

class GeoDataPolygon;

class QAction;

class vtkActor;
class vtkPoints;
class vtkPolyData;

class GeoDataPolygonGroup::Impl
{
public:
	enum Mode {
		Normal,
		EditPolygon,
	};

	Impl(GeoDataPolygonGroup* group);
	~Impl();

	void updateActorSetting();
	void updateActorSettingForEditTargetPolygon();
	void updateSelectedPolygonsVtkObjects();
	void setupDummyPolygonForMenu();

	void selectPolygonsIncludePoint(const QPointF& point);
	void selectPolygonsIncludedInPolygon(const QPolygonF& polygon);

	GeoDataPolygonGroupPolygon* mergeToThis(GeoDataPolygon* polygon);
	void mergeToThis(GeoDataPolygonGroup* group);

	void setupNewEditTargetPolygon();
	void setupEditTargetPolygonFromSelectedPolygon();
	void updateActionStatus();

	std::vector<GeoDataPolygonGroupPolygon*> m_polygons;
	std::unordered_set<GeoDataPolygonGroupPolygon*> m_selectedPolygons;

	vtkPoints* m_points;

	vtkPolyData* m_edgesPolyData;
	vtkPolyData* m_paintPolyData;

	vtkActor* m_edgesActor;
	vtkActor* m_paintActor;

	vtkPolyData* m_selectedPolygonsEdgesPolyData;
	vtkPolyData* m_selectedPolygonsPointsPolyData;

	vtkActor* m_selectedPolygonsEdgesActor;
	vtkActor* m_selectedPolygonsPointsActor;

	geos::index::quadtree::Quadtree* m_qTree;

	GeoDataPolygonGroupColorSettingDialog::Setting m_colorSetting;

	GeoDataPolygon* m_editTargetPolygon;
	GeoDataPolygon* m_dummyPolygonForMenu;
	ZDepthRange m_depthRange;

	QMenu* m_rightClickingMenu;

	QAction* m_addAction;
	QAction* m_selectAction;
	QAction* m_mergeAction;
	QAction* m_deleteAction;
	QAction* m_editColorSettingAction;

	QPoint m_dragStartPoint;

	Mode m_mode;

	GeoDataPolygonGroup* m_group;
};

#endif // GEODATAPOLYGONGROUP_IMPL_H

