#include "../geodatapolygongroupattributebrowser.h"
#include "geodatapolygongroup_impl.h"

#include <geoio/polygonutil.h>
#include <geodata/polygon/geodatapolygon.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

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
#include <geos/geom/Polygon.h>
#include <geos/index/quadtree/Quadtree.h>

#include <algorithm>

namespace {

std::string VALUE = "value";

bool checkSelectionContinuous(const std::vector<GeoDataPolygonGroupPolygon*>& polys, const std::unordered_set<GeoDataPolygonGroupPolygon*>& selectedPolys, GeoDataPolygon* editTarget)
{
	auto selectedPolysCopy = selectedPolys;

	if (selectedPolys.size() == 0) {
		return (editTarget != nullptr);
	}

	bool sel = false;
	for (int i = 0; i < polys.size(); ++i) {
		auto p = polys.at(i);
		auto it = selectedPolysCopy.find(p);
		if (it != selectedPolysCopy.end()) {
			sel = true;
			selectedPolysCopy.erase(it);
		} else {
			if (! sel) {continue;}

			return (selectedPolysCopy.size() == 0);
		}
	}
	return true;
}

} // namespace

GeoDataPolygonGroup::Impl::Impl(GeoDataPolygonGroup* group) :
	m_points {vtkPoints::New()},
	m_edgesPolyData {vtkPolyData::New()},
	m_paintPolyData {vtkPolyData::New()},
	m_edgesActor {vtkActor::New()},
	m_paintActor {vtkActor::New()},
	m_selectedPolygonsEdgesPolyData {vtkPolyData::New()},
	m_selectedPolygonsPointsPolyData {vtkPolyData::New()},
	m_selectedPolygonsEdgesActor {vtkActor::New()},
	m_selectedPolygonsPointsActor {vtkActor::New()},
	m_qTree {new geos::index::quadtree::Quadtree()},
	m_editTargetPolygon {nullptr},
	m_editTargetPolygonBackup {nullptr},
	m_editTargetPolygonIndex {0},
	m_dummyPolygonForMenu {new GeoDataPolygon(group->parent(), group->creator(), group->gridAttribute())},
	m_rightClickingMenu {new QMenu()},
	m_addAction {new QAction(GeoDataPolygonGroup::tr("&Add New Polygon..."), group)},
	m_selectAction {new QAction(GeoDataPolygonGroup::tr("&Select..."), group)},
	m_mergeAction {new QAction(GeoDataPolygonGroup::tr("&Merge..."), group)},
	m_deleteAction {new QAction(GeoDataPolygonGroup::tr("&Delete"), group)},
	m_editNameAction {new QAction(GeoDataPolygonGroup::tr("Edit &Name..."), group)},
	m_editNameAndValueAction {new QAction(GeoDataPolygonGroup::tr("Edit &Name and Value..."), group)},
	m_editColorSettingAction {new QAction(GeoDataPolygonGroup::tr("Color &Setting..."), group)},
	m_attributeBrowserAction {new QAction(GeoDataPolygonGroup::tr("Show Attribute &Browser"), group)},
	m_moveToTopAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveTop.png"), GeoDataPolygonGroup::tr("Move to top"), group)},
	m_moveToBottomAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveBottom.png"), GeoDataPolygonGroup::tr("Move to bottom"), group)},
	m_moveUpAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveUp.png"), GeoDataPolygonGroup::tr("Move up"), group)},
	m_moveDownAction {new QAction(QIcon(":/libs/guibase/images/iconItemMoveDown.png"), GeoDataPolygonGroup::tr("Move down"), group)},
	m_mode {Normal},
	m_attributeBrowser {new GeoDataPolygonGroupAttributeBrowser(group->preProcessorWindow())},
	m_attributeBrowserIsShown {true},
	m_group {group}
{
	m_points->SetDataTypeToDouble();

	auto mapper = vtkPolyDataMapper::New();
	m_edgesActor->SetMapper(mapper);
	mapper->SetInputData(m_edgesPolyData);
	mapper->Delete();

	mapper = vtkPolyDataMapper::New();
	m_paintActor->SetMapper(mapper);
	mapper->SetInputData(m_paintPolyData);
	mapper->Delete();

	mapper = vtkPolyDataMapper::New();
	m_selectedPolygonsEdgesActor->SetMapper(mapper);
	mapper->SetInputData(m_selectedPolygonsEdgesPolyData);
	mapper->Delete();

	mapper = vtkPolyDataMapper::New();
	m_selectedPolygonsPointsActor->SetMapper(mapper);
	mapper->SetInputData(m_selectedPolygonsPointsPolyData);
	mapper->Delete();

	m_selectedPolygonsEdgesActor->GetProperty()->SetLineWidth(2);
	m_selectedPolygonsPointsActor->GetProperty()->SetPointSize(5);

	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));

	setupDummyPolygonForMenu();
}

GeoDataPolygonGroup::Impl::~Impl()
{
	m_points->Delete();
	m_edgesPolyData->Delete();
	m_paintPolyData->Delete();
	m_edgesActor->Delete();
	m_paintActor->Delete();
	m_selectedPolygonsEdgesPolyData->Delete();
	m_selectedPolygonsPointsPolyData->Delete();
	m_selectedPolygonsEdgesActor->Delete();
	m_selectedPolygonsPointsActor->Delete();

	clear();

	delete m_dummyPolygonForMenu;
	delete m_editTargetPolygon;
	delete m_editTargetPolygonBackup;
	delete m_qTree;
	delete m_rightClickingMenu;
	delete m_attributeBrowser;
}

void GeoDataPolygonGroup::Impl::clear()
{
	for (auto p : m_polygons) {
		delete p;
	}
	m_polygons.clear();
}

void GeoDataPolygonGroup::Impl::updateActorSetting()
{
	// color
	double rate = 0.8;
	QColor c = m_colorSetting.color;
	QColor darkColor;
	darkColor.setRedF(c.redF() * rate);
	darkColor.setGreenF(c.greenF() * rate);
	darkColor.setBlueF(c.blueF() * rate);

	m_edgesActor->GetProperty()->SetColor(darkColor.redF(), darkColor.greenF(), darkColor.blueF());
	m_paintActor->GetProperty()->SetColor(m_colorSetting.color);
	m_selectedPolygonsEdgesActor->GetProperty()->SetColor(darkColor.redF(), darkColor.greenF(), darkColor.blueF());
	m_selectedPolygonsPointsActor->GetProperty()->SetColor(darkColor.redF(), darkColor.greenF(), darkColor.blueF());

	// opacity
	m_paintActor->GetProperty()->SetOpacity(m_colorSetting.opacity);

	// mapping
	bool scalarVisibility = true;
	if (m_colorSetting.mapping == GeoDataPolygonGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	m_edgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	m_paintActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	m_selectedPolygonsEdgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	m_selectedPolygonsPointsActor->GetMapper()->SetScalarVisibility(scalarVisibility);

	updateActorSettingForEditTargetPolygon();
}

void GeoDataPolygonGroup::Impl::updateActorSettingForEditTargetPolygon()
{
	if (m_editTargetPolygon == nullptr) {return;}

	m_editTargetPolygon->setColor(m_colorSetting.color);
	m_editTargetPolygon->setOpacity(m_colorSetting.opacity);
	if (m_colorSetting.mapping == GeoDataPolygonGroupColorSettingDialog::Arbitrary) {
		m_editTargetPolygon->setMapping(GeoDataPolygonColorSettingDialog::Arbitrary);
	} else {
		m_editTargetPolygon->setMapping(GeoDataPolygonColorSettingDialog::Value);
	}
}

void GeoDataPolygonGroup::Impl::updateSelectedPolygonsVtkObjects()
{
	m_selectedPolygonsEdgesPolyData->Initialize();
	m_selectedPolygonsPointsPolyData->Initialize();

	auto edges = vtkSmartPointer<vtkCellArray>::New();
	auto points = vtkSmartPointer<vtkCellArray>::New();

	auto edgeValues = vtkSmartPointer<vtkDoubleArray>::New();
	edgeValues->SetName(VALUE.c_str());
	auto pointValues = vtkSmartPointer<vtkDoubleArray>::New();
	pointValues->SetName(VALUE.c_str());

	for (GeoDataPolygonGroupPolygon* pol : m_selectedPolygons) {
		double v = pol->value().toDouble();
		auto offset = pol->indexOffset();
		vtkIdType pts[2];
		unsigned int idx = 0;
		for (const auto& p : pol->points()) {
			pts[0] = offset + idx;
			points->InsertNextCell(1, &(pts[0]));
			pointValues->InsertNextValue(v);
			++ idx;
		}
		auto lineEdges = pol->lineEdges();
		for (int i = 0; i < lineEdges.size() / 2; ++i) {
			pts[0] = lineEdges.at(i * 2) + offset;
			pts[1] = lineEdges.at(i * 2 + 1) + offset;
			edges->InsertNextCell(2, pts);
			edgeValues->InsertNextValue(v);
		}
	}
	m_selectedPolygonsPointsPolyData->SetPoints(m_points);
	m_selectedPolygonsPointsPolyData->SetVerts(points);
	m_selectedPolygonsPointsPolyData->GetCellData()->AddArray(pointValues);
	m_selectedPolygonsPointsPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());

	m_selectedPolygonsEdgesPolyData->SetPoints(m_points);
	m_selectedPolygonsEdgesPolyData->SetLines(edges);
	m_selectedPolygonsEdgesPolyData->GetCellData()->AddArray(edgeValues);
	m_selectedPolygonsEdgesPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPolygonGroup::Impl::updateAttributeBrowser()
{
	if (! m_attributeBrowser->isVisible()) {return;}
	m_attributeBrowser->update();
}

void GeoDataPolygonGroup::Impl::updateAttributeBrowserSelection()
{
	if (! m_attributeBrowser->isVisible()) {return;}
	m_attributeBrowser->updateSelection();
}

void GeoDataPolygonGroup::Impl::setupDummyPolygonForMenu()
{
	m_dummyPolygonForMenu->editNameAndValueAction()->setEnabled(false);
	m_dummyPolygonForMenu->editNameAction()->setEnabled(false);
	m_dummyPolygonForMenu->editValueAction()->setEnabled(false);
	m_dummyPolygonForMenu->addVertexAction()->setEnabled(false);
	m_dummyPolygonForMenu->removeVertexAction()->setEnabled(false);
	m_dummyPolygonForMenu->coordEditAction()->setEnabled(false);
	m_dummyPolygonForMenu->holeModeAction()->setEnabled(false);
	m_dummyPolygonForMenu->deleteAction()->setEnabled(false);
}

void GeoDataPolygonGroup::Impl::selectPolygonsIncludePoint(const QPointF& point)
{
	m_selectedPolygons.clear();
	auto env = new geos::geom::Envelope(point.x(), point.x(), point.y(), point.y());
	std::vector<void*> qret;
	m_qTree->query(env, qret);
	delete env;

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolygonGroupPolygon*> (vptr);
		if (geom == nullptr) {continue;}
		if (geom->isInside(point)) {
			m_selectedPolygons.insert(geom);
		}
	}
	updateSelectedPolygonsVtkObjects();
	updateAttributeBrowser();
	updateActionStatus();
	m_group->updateMenu();
}

void GeoDataPolygonGroup::Impl::selectPolygonsIncludedInPolygon(const QPolygonF& polygon)
{
	m_selectedPolygons.clear();

	auto geosPol = PolygonUtil::getGeosPolygon(polygon);
	auto env = geosPol->getEnvelopeInternal();
	std::vector<void*> qret;
	m_qTree->query(env, qret);

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolygonGroupPolygon*> (vptr);
		if (geom == nullptr) {continue;}
		if (geosPol->contains(geom->geosPolygon())) {
			m_selectedPolygons.insert(geom);
		}
	}
	delete geosPol;

	updateSelectedPolygonsVtkObjects();
	updateAttributeBrowser();
	updateActionStatus();
	m_group->updateMenu();
}

GeoDataPolygonGroupPolygon* GeoDataPolygonGroup::Impl::mergeToThis(GeoDataPolygon* polygon, int index)
{
	auto geosPolygon = polygon->getGeosPolygon(QPointF(0, 0));
	auto p = new GeoDataPolygonGroupPolygon(geosPolygon);
	p->setName(polygon->caption());
	p->setValue(polygon->variantValue());

	if (index == -1) {
		m_polygons.push_back(p);
	} else {
		m_polygons.insert(m_polygons.begin() + index, p);
	}
	return p;
}

void GeoDataPolygonGroup::Impl::mergeToThis(GeoDataPolygonGroup* group)
{
	for (auto p : group->impl->m_polygons) {
		m_polygons.push_back(p);
	}
	group->impl->m_polygons.clear();
}

void GeoDataPolygonGroup::Impl::setupNewEditTargetPolygon()
{
	auto p = new GeoDataPolygon(m_group->parent(), m_group->creator(), m_group->gridAttribute());
	m_editTargetPolygon = p;
	p->setCaption(GeoDataPolygonGroup::tr("Polygon%1").arg(m_polygons.size() + 1));
	p->assignActorZValues(m_depthRange);
	p->informSelection(m_group->graphicsView());
	p->updateActionStatus();
	p->showInitialDialog();
	connect(p, SIGNAL(nameAndValueEdited()), m_group, SLOT(updateAttributeBrowser()));

	m_editTargetPolygonIndex = 0;
	m_editTargetPolygonBackup = new GeoDataPolygonGroupPolygon();
	m_mode = EditPolygon;

	m_selectedPolygons.clear();
	updateActionStatus();
	updateSelectedPolygonsVtkObjects();
	updateActorSettingForEditTargetPolygon();
	updateAttributeBrowser();
}

void GeoDataPolygonGroup::Impl::setupEditTargetPolygonFromSelectedPolygon()
{
	auto selectedPol = *(m_selectedPolygons.begin());
	auto it = std::find(m_polygons.begin(), m_polygons.end(), selectedPol);
	if (it == m_polygons.end()) {return;}

	m_editTargetPolygonIndex = static_cast<int>(it - m_polygons.begin());
	m_polygons.erase(it);

	m_editTargetPolygonBackup = selectedPol;
	auto p = new GeoDataPolygon(m_group->parent(), m_group->creator(), m_group->gridAttribute());
	m_editTargetPolygon = p;
	p->assignActorZValues(m_depthRange);
	p->setCaption(selectedPol->name());
	p->setVariantValue(selectedPol->value());
	p->setShape(selectedPol->geosPolygon(), selectedPol->triangleCells());
	p->setMouseEventMode(GeoDataPolygon::meNormal);
	p->updateActionStatus();
	p->informSelection(m_group->graphicsView());
	connect(p, SIGNAL(nameAndValueEdited()), m_group, SLOT(updateAttributeBrowser()));

	m_selectedPolygons.clear();
	m_mode = EditPolygon;

	m_group->updateVtkObjects();
	m_group->updateIndex();
	m_group->updateMenu();
	updateSelectedPolygonsVtkObjects();
	updateAttributeBrowserSelection();
	updateActorSettingForEditTargetPolygon();

	iRICUndoStack::instance().clear();
}

void GeoDataPolygonGroup::Impl::updateActionStatus()
{
	bool selected = m_selectedPolygons.size() > 0 || (m_editTargetPolygon != nullptr);
	bool continuous = checkSelectionContinuous(m_polygons, m_selectedPolygons, m_editTargetPolygon);

	m_deleteAction->setEnabled(selected);
	m_moveToTopAction->setEnabled(selected);
	m_moveToBottomAction->setEnabled(selected);
	m_moveUpAction->setEnabled(selected && continuous);
	m_moveDownAction->setEnabled(selected && continuous);
}
