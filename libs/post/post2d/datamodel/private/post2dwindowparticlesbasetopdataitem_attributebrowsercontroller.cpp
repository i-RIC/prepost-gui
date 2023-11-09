#include "../post2dwindowzonedataitem.h"
#include "post2dwindowparticlesbasetopdataitem_attributebrowsercontroller.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4particles2d.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/mathsupport.h>

Post2dWindowParticlesBaseTopDataItem::AttributeBrowserController::AttributeBrowserController(Post2dWindowParticlesBaseTopDataItem* item) :
	Post2dWindowAttributeBrowserController {item}
{}

void Post2dWindowParticlesBaseTopDataItem::AttributeBrowserController::initialize()
{
	propertyBrowserView()->resetForParticle();
}

bool Post2dWindowParticlesBaseTopDataItem::AttributeBrowserController::doUpdate(const QPoint& p, VTKGraphicsView* v)
{
	auto particles = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*>(item())->particleData();
	if (particles == nullptr) {return false;}

	auto pos = worldPos(p, v);
	auto pointId = particles->vtkConcreteData()->findClosestPointWithinRadius(v->stdDistance(iRIC::nearRadius()), pos.x(), pos.y(), 0);
	if (pointId < 0) {
		propertyBrowserView()->resetAttributes();
		return false;
	}

	auto pointCoords = particles->point2d(pointId);
	auto attributes = buildAttributes(pointId, particles->vtkData()->data()->GetPointData(),
																		particles->attributeDataProvider());
	propertyBrowserView()->setParticleAttributes(pointId, pointCoords.x(), pointCoords.y(), attributes);
	return true;
}
