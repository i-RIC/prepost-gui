#include "geodatapointgroup_impl.h"

#include <geodata/point/geodatapoint.h>
#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <misc/iricundostack.h>

#include <QAction>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

#include <geos/geom/Envelope.h>
#include <geos/geom/Point.h>

#include <algorithm>

GeoDataPointGroup::Impl::Impl(GeoDataPointGroup* group) :
	m_vtkPoints {vtkPoints::New()},
	m_pointsPolyData {vtkPolyData::New()},
	m_pointsActor {vtkActor::New()},
	m_selectedPointsPointsPolyData {vtkPolyData::New()},
	m_selectedPointsPointsActor {vtkActor::New()},
	m_dummyPointForMenu {new GeoDataPoint(group->parent(), group->creator(), group->gridAttribute())}
{
	m_vtkPoints->SetDataTypeToDouble();

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_pointsActor->SetMapper(mapper);
	mapper->SetInputData(m_pointsPolyData);

	m_pointsActor->GetProperty()->SetPointSize(3);

	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_selectedPointsPointsActor->SetMapper(mapper);
	mapper->SetInputData(m_selectedPointsPointsPolyData);

	m_selectedPointsPointsActor->GetProperty()->SetPointSize(5);

	setupDummyPointForMenu();
}

GeoDataPointGroup::Impl::~Impl()
{
	for (auto actor : m_imageActors) {
		actor->Delete();
	}

	m_vtkPoints->Delete();
	m_pointsPolyData->Delete();
	m_pointsActor->Delete();
	m_selectedPointsPointsPolyData->Delete();
	m_selectedPointsPointsActor->Delete();

	delete m_dummyPointForMenu;
}

QPixmap GeoDataPointGroup::Impl::shrinkPixmap(const QPixmap pixmap, int maxSize)
{
	if (pixmap.width() <= maxSize && pixmap.height() <= maxSize) {return pixmap;}

	if (pixmap.width() > pixmap.height()) {
		return pixmap.scaledToWidth(maxSize);
	} else {
		return pixmap.scaledToHeight(maxSize);
	}
}

void GeoDataPointGroup::Impl::setupDummyPointForMenu()
{
	m_dummyPointForMenu->editNameAndValueAction()->setEnabled(false);
	m_dummyPointForMenu->editNameAction()->setEnabled(false);
	m_dummyPointForMenu->editValueAction()->setEnabled(false);
	m_dummyPointForMenu->coordEditAction()->setEnabled(false);
}
