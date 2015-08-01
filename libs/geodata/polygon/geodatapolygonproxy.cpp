#include "geodatapolygonproxy.h"

#include <vtkProperty.h>
#include <vtkRenderer.h>

void GeoDataPolygonProxy::setupActors()
{
	GeoDataPolygon* pol = dynamic_cast<GeoDataPolygon*>(m_geoData);
	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->SetProperty(pol->m_paintActor->GetProperty());
	m_paintActor->SetMapper(pol->m_paintMapper);

	renderer()->AddActor(m_paintActor);
	actorCollection()->AddItem(m_paintActor);
	updateGraphics();
}

void GeoDataPolygonProxy::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataPolygonProxy::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void GeoDataPolygonProxy::updateGraphics()
{}

void GeoDataPolygonProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolygonProxy::assignActorZValues(const ZDepthRange& range)
{
	m_paintActor->SetPosition(0, 0, range.min());
}
