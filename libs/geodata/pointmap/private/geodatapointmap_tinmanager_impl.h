#ifndef GEODATAPOINTMAP_TINMANAGER_IMPL_H
#define GEODATAPOINTMAP_TINMANAGER_IMPL_H

#include "geodatapointmap_tinmanager.h"
#include "geodatapointmap_tinmanager_removetrianglessetting.h"

class GeoDataPointmap::TINManager::Impl
{
public:
	Impl(TINManager* tinManager, GeoDataPointmap* parent);
	~Impl();

	std::vector<BreakLine*> m_breakLines;
	vtkPolyData* m_tin;
	vtkLODActor* m_tinActor;
	geos::index::quadtree::Quadtree* m_qTree;
	bool m_needRebuild;

	RemoveTrianglesSetting m_removeTrianglesSetting;

	PointsManager* m_pointsManager;

	MouseEventController* m_normalController;
	AddBreakLineController* m_addBreakLineController;
	DeleteBreakLineController* m_deleteBreakLineController;
	std::vector<MouseEventController*> m_controllers;
	MouseEventController* m_activeController;

	GeoDataPointmap* m_parent;
};

#endif // GEODATAPOINTMAP_TINMANAGER_IMPL_H
