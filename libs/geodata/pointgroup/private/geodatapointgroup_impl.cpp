#include "geodatapointgroup_impl.h"

#include <geodata/point/geodatapoint.h>
#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

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
	m_dummyPointForMenu {new GeoDataPoint(group->parent(), group->creator(), group->gridAttribute())},
	m_group {group}
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

QPixmap GeoDataPointGroup::Impl::shrinkPixmap(const QPixmap& pixmap, const DisplaySetting& s, VTKGraphicsView* view)
{
	auto shrinked = Impl::shrinkPixmap(pixmap, s.imageMaxSize);
	auto scale = view->getScale();

	auto imageScale = calcImageScale(scale, s);
	auto width = static_cast<int> (pixmap.width() * imageScale);
	auto height = static_cast<int> (pixmap.height() * imageScale);

	return shrinked.scaled(width, height);
}

double GeoDataPointGroup::Impl::calcImageScale(double scale, const DisplaySetting& s)
{
	if (scale < s.scaleSizePairs.begin()->scale) {
		return s.scaleSizePairs.begin()->size * 0.01;
	}
	if (scale > s.scaleSizePairs.rbegin()->scale) {
		return s.scaleSizePairs.rbegin()->size * 0.01;
	}
	for (int i = 0; i < s.scaleSizePairs.size() - 1; ++i) {
		auto pair1 = s.scaleSizePairs.at(i);
		auto pair2 = s.scaleSizePairs.at(i + 1);

		if (pair1.scale <= scale && scale <= pair2.scale) {
			auto r = (scale - pair1.scale) / (pair2.scale - pair1.scale);
			return (pair1.size + r * (pair2.size - pair1.size)) * 0.01;
		}
	}
	return 1.0;
}

QPixmap GeoDataPointGroup::Impl::shrinkPixmap(const QPixmap& pixmap, int maxSize)
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

QPointF GeoDataPointGroup::Impl::buildBottomLeftCorner(const QPointF& pos, const QPixmap& pixmap, DisplaySetting::AnchorPosition anchor, VTK2DGraphicsView* view)
{
	double d = view->stdDistance(1);
	QPointF delta;
	if (anchor == DisplaySetting::AnchorPosition::BottomLeft) {
		delta = QPointF(0, 0);
	} else if (anchor == DisplaySetting::AnchorPosition::Bottom) {
		delta = QPointF(pixmap.width() * 0.5 * d, 0);
	} else if (anchor == DisplaySetting::AnchorPosition::BottomRight) {
		delta = QPointF(pixmap.width() * d, 0);
	} else if (anchor == DisplaySetting::AnchorPosition::Left) {
		delta = QPointF(0, pixmap.height() * 0.5 * d);
	} else if (anchor == DisplaySetting::AnchorPosition::Center) {
		delta = QPointF(pixmap.width() * 0.5 * d, pixmap.height() * 0.5 * d);
	} else if (anchor == DisplaySetting::AnchorPosition::Right) {
		delta = QPointF(pixmap.width() * d, pixmap.height() * 0.5 * d);
	} else if (anchor == DisplaySetting::AnchorPosition::TopLeft) {
		delta = QPointF(0, pixmap.height() * d);
	} else if (anchor == DisplaySetting::AnchorPosition::Top) {
		delta = QPointF(pixmap.width() * 0.5 * d, pixmap.height() * d);
	} else if (anchor == DisplaySetting::AnchorPosition::TopRight) {
		delta = QPointF(pixmap.width() * d, pixmap.height() * d);
	}
	auto delta2 = iRIC::rotateVector(delta, - view->getAngle());

	return pos - delta2;
}
