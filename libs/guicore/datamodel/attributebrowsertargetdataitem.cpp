#include "attributebrowsertargetdataitem.h"
#include "vtkgraphicsview.h"

#include <guibase/graphicsmisc.h>
#include <misc/errormessage.h>

#include <QPolygonF>
#include <QVector2D>

#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkPolyLine.h>
#include <vtkProperty.h>
#include <vtkQuad.h>
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
	r->RemoveActor(m_polygonController.linesActor());
	r->RemoveActor(m_polygonController.paintActor());
	r->RemoveActor(m_polylineController.pointsActor());
	r->RemoveActor(m_polylineController.linesActor());
}

void AttributeBrowserTargetDataItem::setupActors()
{
	auto r = renderer();
	m_polygonController.paintActor()->GetProperty()->SetColor(0, 0, 0);
	m_polygonController.paintActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_polygonController.paintActor());

	m_polygonController.linesActor()->GetProperty()->SetLineWidth(2);
	m_polygonController.linesActor()->GetProperty()->SetColor(0, 0, 0);
	m_polygonController.linesActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_polygonController.linesActor());

	m_polylineController.pointsActor()->GetProperty()->SetPointSize(7);
	m_polylineController.pointsActor()->GetProperty()->SetColor(0, 0, 0);
	m_polylineController.pointsActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_polylineController.pointsActor());

	m_polylineController.linesActor()->GetProperty()->SetLineWidth(2);
	m_polylineController.linesActor()->GetProperty()->SetColor(0, 0, 0);
	m_polylineController.linesActor()->GetProperty()->SetOpacity(0.6);
	r->AddActor(m_polylineController.linesActor());
}

void AttributeBrowserTargetDataItem::setPoint(const QPointF& v)
{
	m_polygonController.linesActor()->VisibilityOff();
	m_polygonController.paintActor()->VisibilityOff();
	m_polylineController.linesActor()->VisibilityOff();
	m_polylineController.pointsActor()->VisibilityOn();

	std::vector<QPointF> points;
	points.push_back(v);
	m_polylineController.setPolyLine(points);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setPolygon(const QPolygonF& p)
{
	m_polygonController.linesActor()->VisibilityOff();
	m_polygonController.paintActor()->VisibilityOff();
	m_polylineController.linesActor()->VisibilityOff();
	m_polylineController.pointsActor()->VisibilityOff();

	if (p.size() == 0) {
		return;
	}
	if (! p.isClosed()) {
		throw ErrorMessage(tr("Please specify a closed polygon!"));
	}
	m_polygonController.linesActor()->VisibilityOn();
	m_polygonController.paintActor()->VisibilityOn();
	m_polygonController.setPolygon(p);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setPolyline(const std::vector<QPointF>& l)
{
	m_polygonController.linesActor()->VisibilityOff();
	m_polygonController.paintActor()->VisibilityOff();
	m_polylineController.linesActor()->VisibilityOn();
	m_polylineController.pointsActor()->VisibilityOff();

	m_polylineController.setPolyLine(l);

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::setCell(vtkCell* cell)
{
	auto polyline = dynamic_cast<vtkPolyLine*>(cell);

	if (polyline != nullptr) {
		auto points = cell->GetPoints();
		auto ids = cell->GetPointIds();
		std::vector<QPointF> l;
		for (vtkIdType i = 0; i < ids->GetNumberOfIds(); ++i) {
			double v[3];
			points->GetPoint(i, v);
			l.push_back(QPointF(v[0], v[1]));
		}
		setPolyline(l);
	} else {
		auto points = cell->GetPoints();
		auto ids = cell->GetPointIds();
		QPolygonF p;
		for (vtkIdType i = 0; i < ids->GetNumberOfIds(); ++i) {
			double v[3];
			points->GetPoint(i, v);
			p.push_back(QPointF(v[0], v[1]));
		}
		double v[3];
		points->GetPoint(0, v);
		p.push_back(QPointF(v[0], v[1]));
		setPolygon(p);
	}
	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::clear()
{
	m_polygonController.linesActor()->VisibilityOff();
	m_polygonController.paintActor()->VisibilityOff();
	m_polylineController.linesActor()->VisibilityOff();
	m_polylineController.pointsActor()->VisibilityOff();

	renderGraphicsView();
}

void AttributeBrowserTargetDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void AttributeBrowserTargetDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_polygonController.paintActor()->SetPosition(0, 0, range.min());
	m_polygonController.linesActor()->SetPosition(0, 0, range.max());
	m_polylineController.linesActor()->SetPosition(0, 0, range.max());
	m_polylineController.pointsActor()->SetPosition(0, 0, range.max());
}

void AttributeBrowserTargetDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void AttributeBrowserTargetDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
