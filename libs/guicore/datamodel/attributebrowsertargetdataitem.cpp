#include "attributebrowsertargetdataitem.h"
#include "vtkgraphicsview.h"

#include <guibase/graphicsmisc.h>
#include <misc/errormessage.h>

#include <QPolygonF>
#include <QVector2D>

#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>

AttributeBrowserTargetDataItem::AttributeBrowserTargetDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(parent)
{
	setupActors();

	clear();
}

AttributeBrowserTargetDataItem::~AttributeBrowserTargetDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_polygonsActor.linesActor());
	r->RemoveActor(m_polygonsActor.paintActor());
	r->RemoveActor(m_lineActor.pointsActor());
	r->RemoveActor(m_lineActor.lineActor());
}

void AttributeBrowserTargetDataItem::setupActors()
{
	auto r = renderer();
	m_polygonsActor.paintActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
	m_polygonsActor.paintActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_polygonsActor.paintActor());

	m_polygonsActor.linesActor()->GetProperty()->SetLineWidth(2);
	m_polygonsActor.linesActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
	m_polygonsActor.linesActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_polygonsActor.linesActor());

	m_lineActor.pointsActor()->GetProperty()->SetPointSize(7);
	m_lineActor.pointsActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
	m_lineActor.pointsActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_lineActor.pointsActor());

	m_lineActor.lineActor()->GetProperty()->SetLineWidth(5);
	m_lineActor.lineActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
	m_lineActor.lineActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_lineActor.lineActor());
}

void AttributeBrowserTargetDataItem::setPoint(const QPointF& v)
{
	m_polygonsActor.linesActor()->VisibilityOff();
	m_polygonsActor.paintActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOff();
	m_lineActor.pointsActor()->VisibilityOn();

	std::vector<QPointF> points;
	points.push_back(v);
	m_lineActor.setLine(points);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setPolygon(const QPolygonF& p)
{
	m_polygonsActor.linesActor()->VisibilityOff();
	m_polygonsActor.paintActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOff();
	m_lineActor.pointsActor()->VisibilityOff();

	if (p.size() == 0) {
		return;
	}
	if (! p.isClosed()) {
		throw ErrorMessage(tr("Please specify a closed polygon!"));
	}
	m_polygonsActor.linesActor()->VisibilityOn();
	m_polygonsActor.paintActor()->VisibilityOn();
	m_polygonsActor.setPolygon(p);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setPolygons(const std::vector<QPolygonF>& p)
{
	m_polygonsActor.linesActor()->VisibilityOff();
	m_polygonsActor.paintActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOff();
	m_lineActor.pointsActor()->VisibilityOff();

	if (p.size() == 0) {
		return;
	}

	m_polygonsActor.paintActor()->VisibilityOn();
	m_polygonsActor.setPolygons(p);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setPolyline(const std::vector<QPointF>& l)
{
	m_polygonsActor.linesActor()->VisibilityOff();
	m_polygonsActor.paintActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOn();
	m_lineActor.pointsActor()->VisibilityOff();

	m_lineActor.setLine(l);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::clear()
{
	m_polygonsActor.linesActor()->VisibilityOff();
	m_polygonsActor.paintActor()->VisibilityOff();
	m_lineActor.lineActor()->VisibilityOff();
	m_lineActor.pointsActor()->VisibilityOff();

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void AttributeBrowserTargetDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_polygonsActor.paintActor()->SetPosition(0, 0, range.min());
	m_polygonsActor.linesActor()->SetPosition(0, 0, range.max());
	m_lineActor.lineActor()->SetPosition(0, 0, range.max());
	m_lineActor.pointsActor()->SetPosition(0, 0, range.max());
}

void AttributeBrowserTargetDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void AttributeBrowserTargetDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
