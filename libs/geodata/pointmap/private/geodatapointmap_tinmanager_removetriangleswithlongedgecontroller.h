#ifndef GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESWITHLONGEDGECONTROLLER_H
#define GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESWITHLONGEDGECONTROLLER_H

#include "geodatapointmap_tinmanager.h"

#include <misc/mouseeventcontroller.h>

class GeoDataPointmap::TINManager::RemoveTrianglesWithLongEdgeController : public MouseEventController
{
public:
	RemoveTrianglesWithLongEdgeController(TINManager);

	void activate(VTK2DGraphicsView* v) override;
	void deactivate(VTK2DGraphicsView* v) override;

	TINManager* m_manager;
};

#endif // GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESWITHLONGEDGECONTROLLER_H
