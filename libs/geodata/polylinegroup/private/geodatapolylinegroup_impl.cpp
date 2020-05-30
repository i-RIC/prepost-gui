#include "geodatapolylinegroup_impl.h"

#include <geoio/polygonutil.h>
#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <geodata/polyline/geodatapolyline.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

#include <QAction>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

#include <geos/geom/Envelope.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/index/quadtree/Quadtree.h>

#include <algorithm>

GeoDataPolyLineGroup::Impl::Impl(GeoDataPolyLineGroup* group) :
	m_points {vtkPoints::New()},
	m_edgesPolyData {vtkPolyData::New()},
	m_edgesActor {vtkActor::New()},
	m_selectedPolyLinesEdgesPolyData {vtkPolyData::New()},
	m_selectedPolyLinesPointsPolyData {vtkPolyData::New()},
	m_selectedPolyLinesEdgesActor {vtkActor::New()},
	m_selectedPolyLinesPointsActor {vtkActor::New()},
	m_dummyPolyLineForMenu {new GeoDataPolyLine(group->parent(), group->creator(), group->gridAttribute())}
{
	m_points->SetDataTypeToDouble();

	auto mapper = vtkPolyDataMapper::New();
	m_edgesActor->SetMapper(mapper);
	mapper->SetInputData(m_edgesPolyData);
	mapper->Delete();

	mapper = vtkPolyDataMapper::New();
	m_selectedPolyLinesEdgesActor->SetMapper(mapper);
	mapper->SetInputData(m_selectedPolyLinesEdgesPolyData);
	mapper->Delete();

	mapper = vtkPolyDataMapper::New();
	m_selectedPolyLinesPointsActor->SetMapper(mapper);
	mapper->SetInputData(m_selectedPolyLinesPointsPolyData);
	mapper->Delete();

	m_selectedPolyLinesEdgesActor->GetProperty()->SetLineWidth(2);
	m_selectedPolyLinesPointsActor->GetProperty()->SetPointSize(5);

	setupDummyPolyLineForMenu();
}

GeoDataPolyLineGroup::Impl::~Impl()
{
	m_points->Delete();
	m_edgesPolyData->Delete();
	m_edgesActor->Delete();
	m_selectedPolyLinesEdgesPolyData->Delete();
	m_selectedPolyLinesPointsPolyData->Delete();
	m_selectedPolyLinesEdgesActor->Delete();
	m_selectedPolyLinesPointsActor->Delete();

	delete m_dummyPolyLineForMenu;
}

void GeoDataPolyLineGroup::Impl::setupDummyPolyLineForMenu()
{
	m_dummyPolyLineForMenu->editNameAndValueAction()->setEnabled(false);
	m_dummyPolyLineForMenu->editNameAction()->setEnabled(false);
	m_dummyPolyLineForMenu->editValueAction()->setEnabled(false);
	m_dummyPolyLineForMenu->addVertexAction()->setEnabled(false);
	m_dummyPolyLineForMenu->removeVertexAction()->setEnabled(false);
	m_dummyPolyLineForMenu->coordEditAction()->setEnabled(false);
}

/*
QString GeoDataPolyLineGroup::Impl::createNewId() const
{
	QString tmpl("polyLine%1");
	int idx = 1;
	QString newId = tmpl.arg(idx);
	while (m_polyLineIdMap.find(newId) != m_polyLineIdMap.end()) {
		newId = tmpl.arg(++idx);
	}
	return newId;
}
*/
