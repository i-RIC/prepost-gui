#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "private/geodatapointgroup_impl.h"

#include <geodata/point/geodatapoint.h>
#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <geoio/polygonutil.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/mathsupport.h>
#include <misc/zdepthrange.h>

#include <QMenu>

#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/index/quadtree/Quadtree.h>

#include <string>

namespace {

std::string VALUE = "value";

} // namespace

GeoDataPointGroup::GeoDataPointGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataPolyDataGroup(d, creator, condition),
	impl {new Impl {this}}
{
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		auto mapper = impl->m_pointsActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPointsPointsActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}

	actorCollection()->AddItem(impl->m_pointsActor);

	renderer()->AddActor(impl->m_pointsActor);
	renderer()->AddActor(impl->m_selectedPointsPointsActor);

	updateActorSetting();
}

GeoDataPointGroup::~GeoDataPointGroup()
{
	actorCollection()->RemoveAllItems();
	renderer()->RemoveActor(impl->m_pointsActor);
	renderer()->RemoveActor(impl->m_selectedPointsPointsActor);

	delete impl;
}

std::vector<GeoDataPointGroupPoint*> GeoDataPointGroup::pointsInBoundingBox(double xmin, double xmax, double ymin, double ymax) const
{
	auto env = new geos::geom::Envelope(xmin, xmax, ymin, ymax);
	std::vector<void*> qret;
	qTree()->query(env, qret);
	delete env;

	std::vector<GeoDataPointGroupPoint*> ret;
	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPointGroupPoint*> (vptr);
		if (geom == nullptr) {continue;}
		double gxmin, gxmax, gymin, gymax;
		geom->getBoundingRect(&gxmin, &gxmax, &gymin, &gymax);
		if (gxmax < xmin) {continue;}
		if (gxmin > xmax) {continue;}
		if (gymax < ymin) {continue;}
		if (gymin > ymax) {continue;}

		ret.push_back(geom);
	}
	return ret;
}

void GeoDataPointGroup::updateVtkObjects()
{
	impl->m_vtkPoints->Initialize();
	impl->m_pointsPolyData->Initialize();

	auto points = vtkSmartPointer<vtkCellArray>::New();
	auto pointValues = vtkSmartPointer<vtkDoubleArray>::New();
	pointValues->SetName(VALUE.c_str());

	vtkIdType id = 0;
	for (auto it = data().rbegin(); it != data().rend(); ++it) {
		auto point = dynamic_cast<GeoDataPointGroupPoint*> (*it);
		double v = point->value().toDouble();
		auto p = point->point();
		impl->m_vtkPoints->InsertNextPoint(p.x(), p.y(), 0);
		points->InsertNextCell(1, &id);
		pointValues->InsertNextValue(v);
		point->setIndexOffset(id);
		++ id;
	}
	impl->m_pointsPolyData->SetPoints(impl->m_vtkPoints);
	impl->m_pointsPolyData->SetVerts(points);
	impl->m_pointsPolyData->GetCellData()->AddArray(pointValues);
	impl->m_pointsPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPointGroup::updateSelectedDataVtkObjects()
{
	impl->m_selectedPointsPointsPolyData->Initialize();

	auto points = vtkSmartPointer<vtkCellArray>::New();

	auto pointValues = vtkSmartPointer<vtkDoubleArray>::New();
	pointValues->SetName(VALUE.c_str());

	for (auto d : selectedData()) {
		auto point = dynamic_cast<GeoDataPointGroupPoint*> (d);
		double v = point->value().toDouble();
		vtkIdType id = point->indexOffset();
		points->InsertNextCell(1, &id);
		pointValues->InsertNextValue(v);
	}
	impl->m_selectedPointsPointsPolyData->SetPoints(impl->m_vtkPoints);
	impl->m_selectedPointsPointsPolyData->SetVerts(points);
	impl->m_selectedPointsPointsPolyData->GetCellData()->AddArray(pointValues);
	impl->m_selectedPointsPointsPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPointGroup::selectDataWithPoint(const QPointF& point)
{
	auto v = graphicsView();
	double r = v->stdRadius(iRIC::nearRadius());
	QPolygonF pol;
	pol.push_back(point + QPointF(-r, -r));
	pol.push_back(point + QPointF( r, -r));
	pol.push_back(point + QPointF( r,  r));
	pol.push_back(point + QPointF(-r,  r));
	pol.push_back(point + QPointF(-r, -r));
	selectDataWithPolygon(pol);
}

void GeoDataPointGroup::selectDataWithPolygon(const QPolygonF& polygon)
{
	auto& selData = selectedData();
	selData.clear();

	auto geosPol = PolygonUtil::getGeosPolygon(polygon);
	auto env = geosPol->getEnvelopeInternal();
	std::vector<void*> qret;
	qTree()->query(env, qret);

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPointGroupPoint*> (vptr);
		if (geom == nullptr) {continue;}
		if (polygon.containsPoint(geom->point(), Qt::OddEvenFill)) {
			selData.insert(geom);
		}
	}
	delete geosPol;
}

std::vector<vtkActor*> GeoDataPointGroup::actorsToShowWhenSelected()
{
	std::vector<vtkActor*> ret;
	ret.push_back(impl->m_selectedPointsPointsActor);
	return ret;
}

bool GeoDataPointGroup::isMergablePolyData(GeoData* geoData)
{
	return dynamic_cast<GeoDataPoint*> (geoData) != nullptr;
}

bool GeoDataPointGroup::isMergablePolyDataGroup(GeoData* geoData)
{
	return dynamic_cast<GeoDataPointGroup*> (geoData) != nullptr;
}

QString GeoDataPointGroup::captionForData(int number)
{
	return tr("Point%1").arg(number);
}

void GeoDataPointGroup::setupMenu()
{
	m_menu->setTitle(tr("P&oint Group"));

	updateMenu();
}

void GeoDataPointGroup::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPointGroup::assignActorZValues(const ZDepthRange& range)
{
	GeoDataPolyDataGroup::assignActorZValues(range);

	impl->m_pointsActor->SetPosition(0, 0, range.max());
	impl->m_selectedPointsPointsActor->SetPosition(0, 0, range.max());
}

GeoDataPolyDataGroupPolyData* GeoDataPointGroup::createNewData()
{
	return new GeoDataPointGroupPoint(this);
}

GeoDataPolyData* GeoDataPointGroup::createEditTargetData()
{
	auto point = new GeoDataPoint(parent(), creator(), gridAttribute());

	point->assignActorZValues(depthRange());
	connect(point, SIGNAL(nameAndValueEdited()), this, SLOT(updateAttributeBrowser()));

	return point;
}

void GeoDataPointGroup::updateActorSetting()
{
	auto cs = colorSetting();

	// color
	QColor c = cs.color;

	impl->m_pointsActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_selectedPointsPointsActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPolyDataGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	impl->m_pointsActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_selectedPointsPointsActor->GetMapper()->SetScalarVisibility(scalarVisibility);

	// opacity
	impl->m_pointsActor->GetProperty()->SetOpacity(cs.opacity);

	updateActorSettingForEditTargetPolyData();
}

void GeoDataPointGroup::updateMenu()
{
	auto att = gridAttribute();
	bool isRef = att && att->isReferenceInformation();
	GeoDataPoint* p = impl->m_dummyPointForMenu;
	if (editTargetData() != nullptr) {
		p = dynamic_cast<GeoDataPoint*> (editTargetData());
	}

	auto m = m_menu;

	m->clear();

	m->addAction(editNameAction());

	m->addSeparator();
	m->addAction(addAction());

	m->addSeparator();
	if (selectedData().size() > 1) {
		if (isRef) {
			m->addAction(editNameAction());
		} else {
			m->addAction(editNameAndValueAction());
		}
	} else {
		if (isRef) {
			m->addAction(p->editNameAction());
		} else {
			m->addAction(p->editNameAndValueAction());
		}
	}

	m->addSeparator();
	m->addAction(p->coordEditAction());

	m->addSeparator();
	auto sortMenu = m->addMenu(tr("&Sort"));
	sortMenu->addAction(moveToTopAction());
	sortMenu->addAction(moveUpAction());
	sortMenu->addAction(moveDownAction());
	sortMenu->addAction(moveToBottomAction());

	m->addSeparator();
	m->addAction(mergeAction());
	m->addAction(editColorSettingAction());
	m->addAction(attributeBrowserAction());

	m->addSeparator();
	m->addAction(deleteAction());

	// right clicking menu

	m = rightClickingMenu();
	m->clear();

	m->addAction(addAction());

	m->addSeparator();
	if (selectedData().size() > 1) {
		if (isRef) {
			m->addAction(editNameAction());
		} else {
			m->addAction(editNameAndValueAction());
		}
	} else {
		if (isRef) {
			m->addAction(p->editNameAction());
		} else {
			m->addAction(p->editNameAndValueAction());
		}
	}

	m->addSeparator();
	m->addAction(p->coordEditAction());

	m->addSeparator();
	sortMenu = m->addMenu(tr("&Sort"));
	sortMenu->addAction(moveToTopAction());
	sortMenu->addAction(moveUpAction());
	sortMenu->addAction(moveDownAction());
	sortMenu->addAction(moveToBottomAction());

	m->addSeparator();
	m->addAction(mergeAction());
	m->addAction(editColorSettingAction());
	m->addAction(attributeBrowserAction());

	m->addSeparator();
	m->addAction(deleteAction());
}
