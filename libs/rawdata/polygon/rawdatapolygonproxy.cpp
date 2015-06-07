#include "rawdatapolygonproxy.h"

#include <vtkProperty.h>
#include <vtkRenderer.h>

void RawDataPolygonProxy::setupActors()
{
	RawDataPolygon* pol = dynamic_cast<RawDataPolygon*>(m_rawData);
	m_paintActor = vtkSmartPointer<vtkActor>::New();
	m_paintActor->SetProperty(pol->m_paintActor->GetProperty());
	m_paintActor->SetMapper(pol->m_paintMapper);

	renderer()->AddActor(m_paintActor);
	actorCollection()->AddItem(m_paintActor);
	updateGraphics();
}

void RawDataPolygonProxy::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{
}

void RawDataPolygonProxy::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{
}

void RawDataPolygonProxy::updateGraphics()
{
}

void RawDataPolygonProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void RawDataPolygonProxy::assignActorZValues(const ZDepthRange& range)
{
	m_paintActor->SetPosition(0, 0, range.min());
}

