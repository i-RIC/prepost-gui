#include "geodatapolygongroup.h"
#include "geodatapolygongrouppolygon.h"
#include "private/geodatapolygongroup_impl.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <geodata/polygon/geodatapolygon.h>
#include <geoio/polygonutil.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
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

GeoDataPolygonGroup::GeoDataPolygonGroup(ProjectDataItem* d, GeoDataCreator* gdcreator, SolverDefinitionGridAttribute* condition) :
	GeoDataPolyDataGroup(d, gdcreator, condition),
	impl {new Impl {this}}
{
	addAction()->setText(tr("&Add New %1...").arg(creator()->shapeNameCamelCase()));

	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		auto mapper = impl->m_paintActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_edgesActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPolygonsEdgesActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPolygonsPointsActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}

	actorCollection()->AddItem(impl->m_paintActor);
	actorCollection()->AddItem(impl->m_edgesActor);

	renderer()->AddActor(impl->m_paintActor);
	renderer()->AddActor(impl->m_edgesActor);
	renderer()->AddActor(impl->m_selectedPolygonsEdgesActor);
	renderer()->AddActor(impl->m_selectedPolygonsPointsActor);

	updateActorSetting();
}

GeoDataPolygonGroup::~GeoDataPolygonGroup()
{
	actorCollection()->RemoveAllItems();
	renderer()->RemoveActor(impl->m_paintActor);
	renderer()->RemoveActor(impl->m_edgesActor);
	renderer()->RemoveActor(impl->m_selectedPolygonsEdgesActor);
	renderer()->RemoveActor(impl->m_selectedPolygonsPointsActor);

	delete impl;
}

std::vector<GeoDataPolygonGroupPolygon*> GeoDataPolygonGroup::polygonsInBoundingBox(double xmin, double xmax, double ymin, double ymax) const
{
	auto env = new geos::geom::Envelope(xmin, xmax, ymin, ymax);
	std::vector<void*> qret;
	qTree()->query(env, qret);
	delete env;

	std::vector<GeoDataPolygonGroupPolygon*> ret;
	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolygonGroupPolygon*> (vptr);
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

void GeoDataPolygonGroup::updateVtkObjects()
{
	impl->m_points->Initialize();
	impl->m_paintPolyData->Initialize();
	impl->m_edgesPolyData->Initialize();

	auto edges = vtkSmartPointer<vtkCellArray>::New();
	auto tris = vtkSmartPointer<vtkCellArray>::New();

	auto edgeValues = vtkSmartPointer<vtkDoubleArray>::New();
	edgeValues->SetName(VALUE.c_str());
	auto triValues = vtkSmartPointer<vtkDoubleArray>::New();
	triValues->SetName(VALUE.c_str());

	vtkIdType offset = 0;
	for (auto it = data().rbegin(); it != data().rend(); ++it) {
		auto pol = dynamic_cast<GeoDataPolygonGroupPolygon*> (*it);
		double v = pol->value().toDouble();
		for (const QPointF& p : pol->points()) {
			impl->m_points->InsertNextPoint(p.x(), p.y(), 0);
		}
		auto lineEdges = pol->lineEdges();
		vtkIdType pts[3];
		for (int i = 0; i < lineEdges.size() / 2; ++i) {
			pts[0] = lineEdges.at(i * 2) + offset;
			pts[1] = lineEdges.at(i * 2 + 1) + offset;
			edges->InsertNextCell(2, pts);
			edgeValues->InsertNextValue(v);
		}
		auto cells = pol->triangleCells();
		for (int i = 0; i < cells.size() / 3; ++i) {
			pts[0] = cells.at(i * 3) + offset;
			pts[1] = cells.at(i * 3 + 1) + offset;
			pts[2] = cells.at(i * 3 + 2) + offset;
			tris->InsertNextCell(3, pts);
			triValues->InsertNextValue(v);
		}
		pol->setIndexOffset(offset);
		offset = impl->m_points->GetNumberOfPoints();
	}
	impl->m_paintPolyData->SetPoints(impl->m_points);
	impl->m_paintPolyData->SetPolys(tris);
	impl->m_paintPolyData->GetCellData()->AddArray(triValues);
	impl->m_paintPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());

	impl->m_edgesPolyData->SetPoints(impl->m_points);
	impl->m_edgesPolyData->SetLines(edges);
	impl->m_edgesPolyData->GetCellData()->AddArray(edgeValues);
	impl->m_edgesPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPolygonGroup::updateSelectedDataVtkObjects()
{
	impl->m_selectedPolygonsEdgesPolyData->Initialize();
	impl->m_selectedPolygonsPointsPolyData->Initialize();

	auto edges = vtkSmartPointer<vtkCellArray>::New();
	auto points = vtkSmartPointer<vtkCellArray>::New();

	auto edgeValues = vtkSmartPointer<vtkDoubleArray>::New();
	edgeValues->SetName(VALUE.c_str());
	auto pointValues = vtkSmartPointer<vtkDoubleArray>::New();
	pointValues->SetName(VALUE.c_str());

	auto sData = selectedData();
	if (sData.size() == 0) {return;}

	for (auto d : sData) {
		auto pol = dynamic_cast<GeoDataPolygonGroupPolygon*> (d);
		double v = pol->value().toDouble();
		auto offset = pol->indexOffset();
		vtkIdType pts[2];
		for (unsigned int idx = 0; idx < pol->points().size(); ++idx) {
			pts[0] = offset + idx;
			points->InsertNextCell(1, &(pts[0]));
			pointValues->InsertNextValue(v);
		}
		auto lineEdges = pol->lineEdges();
		for (int i = 0; i < lineEdges.size() / 2; ++i) {
			pts[0] = lineEdges.at(i * 2) + offset;
			pts[1] = lineEdges.at(i * 2 + 1) + offset;
			edges->InsertNextCell(2, pts);
			edgeValues->InsertNextValue(v);
		}
	}

	impl->m_selectedPolygonsPointsPolyData->SetPoints(impl->m_points);
	impl->m_selectedPolygonsPointsPolyData->SetVerts(points);
	impl->m_selectedPolygonsPointsPolyData->GetCellData()->AddArray(pointValues);
	impl->m_selectedPolygonsPointsPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());

	impl->m_selectedPolygonsEdgesPolyData->SetPoints(impl->m_points);
	impl->m_selectedPolygonsEdgesPolyData->SetLines(edges);
	impl->m_selectedPolygonsEdgesPolyData->GetCellData()->AddArray(edgeValues);
	impl->m_selectedPolygonsEdgesPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPolygonGroup::selectDataWithPoint(const QPointF& point)
{
	auto& selData = selectedData();
	selData.clear();

	auto env = new geos::geom::Envelope(point.x(), point.x(), point.y(), point.y());
	std::vector<void*> qret;
	qTree()->query(env, qret);
	delete env;

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolygonGroupPolygon*> (vptr);
		if (geom == nullptr) {continue;}
		if (geom->isInside(point)) {
			selData.insert(geom);
		}
	}
}

void GeoDataPolygonGroup::selectDataWithPolygon(const QPolygonF& polygon)
{
	auto& selData = selectedData();
	selData.clear();

	auto geosPol = PolygonUtil::getGeosPolygon(polygon);
	auto env = geosPol->getEnvelopeInternal();
	std::vector<void*> qret;
	qTree()->query(env, qret);

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolygonGroupPolygon*> (vptr);
		if (geom == nullptr) {continue;}
		if (geosPol->contains(geom->geosPolygon())) {
			selData.insert(geom);
		}
	}
	delete geosPol;
}

std::vector<vtkActor*> GeoDataPolygonGroup::actorsToShowWhenSelected()
{
	std::vector<vtkActor*> ret;
	ret.push_back(impl->m_selectedPolygonsEdgesActor);
	ret.push_back(impl->m_selectedPolygonsPointsActor);
	return ret;
}

bool GeoDataPolygonGroup::isMergablePolyData(GeoData* geoData)
{
	return dynamic_cast<GeoDataPolygon*> (geoData) != nullptr;
}

bool GeoDataPolygonGroup::isMergablePolyDataGroup(GeoData* geoData)
{
	return dynamic_cast<GeoDataPolygonGroup*> (geoData) != nullptr;
}

QString GeoDataPolygonGroup::captionForData(int number)
{
	return tr("Polygon%1").arg(number);
}

GeoDataPolyDataGroup* GeoDataPolygonGroup::createInstanceForCopy(ProjectDataItem* d)
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	auto newGroup = new GeoDataPolygonGroup(d, creator(), gItem->condition());

	return newGroup;
}

void GeoDataPolygonGroup::setupMenu()
{
	m_menu->setTitle(tr("P&olygon Group"));

	updateMenu();
}

void GeoDataPolygonGroup::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GeoDataPolygonGroup::assignActorZValues(const ZDepthRange& range)
{
	GeoDataPolyDataGroup::assignActorZValues(range);

	impl->m_edgesActor->SetPosition(0, 0, range.max());
	impl->m_paintActor->SetPosition(0, 0, range.min());
	impl->m_selectedPolygonsEdgesActor->SetPosition(0, 0, range.max());
	impl->m_selectedPolygonsPointsActor->SetPosition(0, 0, range.max());
}

GeoDataPolyDataGroupPolyData* GeoDataPolygonGroup::createNewData()
{
	return new GeoDataPolygonGroupPolygon(this);
}

GeoDataPolyData* GeoDataPolygonGroup::createEditTargetData()
{
	auto polygon = new GeoDataPolygon(parent(), creator(), gridAttribute());

	polygon->assignActorZValues(depthRange());
	connect(polygon, SIGNAL(nameAndValueEdited()), this, SLOT(updateAttributeBrowser()));

	return polygon;
}

void GeoDataPolygonGroup::updateActorSetting()
{
	auto cs = colorSetting();

	// color
	double rate = 0.8;
	QColor c = cs.color;
	QColor darkColor;
	darkColor.setRedF(c.redF() * rate);
	darkColor.setGreenF(c.greenF() * rate);
	darkColor.setBlueF(c.blueF() * rate);

	impl->m_edgesActor->GetProperty()->SetColor(darkColor.redF(), darkColor.greenF(), darkColor.blueF());
	impl->m_paintActor->GetProperty()->SetColor(cs.color);
	impl->m_selectedPolygonsEdgesActor->GetProperty()->SetColor(darkColor.redF(), darkColor.greenF(), darkColor.blueF());
	impl->m_selectedPolygonsPointsActor->GetProperty()->SetColor(darkColor.redF(), darkColor.greenF(), darkColor.blueF());

	// opacity
	impl->m_paintActor->GetProperty()->SetOpacity(cs.opacity);

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPolyDataGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	impl->m_edgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_paintActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_selectedPolygonsEdgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_selectedPolygonsPointsActor->GetMapper()->SetScalarVisibility(scalarVisibility);

	updateActorSettingForEditTargetPolyData();
}

void GeoDataPolygonGroup::updateMenu()
{
	auto att = gridAttribute();
	bool isRef = att && att->isReferenceInformation();
	GeoDataPolygon* p = impl->m_dummyPolygonForMenu;
	if (editTargetData() != nullptr) {
		p = dynamic_cast<GeoDataPolygon*> (editTargetData());
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
	m->addAction(p->addVertexAction());
	m->addAction(p->removeVertexAction());
	m->addAction(p->coordEditAction());

	m->addSeparator();
	m->addAction(p->holeModeAction());
	m->addAction(p->deleteAction());

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
	m->addAction(p->addVertexAction());
	m->addAction(p->removeVertexAction());
	m->addAction(p->coordEditAction());

	m->addSeparator();
	m->addAction(p->holeModeAction());
	m->addAction(p->deleteAction());

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
