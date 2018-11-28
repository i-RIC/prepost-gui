#include "geodatapolylineproxy.h"

#include <misc/zdepthrange.h>

#include <vtkProperty.h>
#include <vtkRenderer.h>

void GeoDataPolyLineProxy::setupActors()
{
	GeoDataPolyLine* pol = dynamic_cast<GeoDataPolyLine*>(geoData());

	updateGraphics();
}

void GeoDataPolyLineProxy::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataPolyLineProxy::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void GeoDataPolyLineProxy::updateGraphics()
{}

void GeoDataPolyLineProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolyLineProxy::assignActorZValues(const ZDepthRange& range)
{
}
