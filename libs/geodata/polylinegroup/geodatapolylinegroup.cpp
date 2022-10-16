#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupcolorsettingdialog.h"
#include "geodatapolylinegroupcreator.h"
#include "geodatapolylinegrouppolyline.h"
#include "geodatapolylinegroupproxy.h"
#include "private/geodatapolylinegroup_impl.h"
#include "private/geodatapolylinegroup_setcolorsettingcommand.h"

#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <geodata/polyline/geodatapolyline.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <geoio/polygonutil.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/geodata/geodatafactoryinterface.h>
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
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/index/quadtree/Quadtree.h>

#include <string>

namespace {

std::string VALUE = "value";

GeoDataCreator* getPolyLineGroupCreator(PreProcessorGeoDataDataItemInterface* geoData, SolverDefinitionGridAttribute* att)
{
		auto factory = geoData->preProcessorWindow()->geoDataFactory();
		auto creators = factory->compatibleCreators(att);
		for (auto c : creators) {
				auto c2 = dynamic_cast <GeoDataPolyLineGroupCreator*> (c);
				if (c2 != nullptr) {return c2;}
		}
		return nullptr;
}

} // namespace

GeoDataPolyLineGroup::GeoDataPolyLineGroup(ProjectDataItem* d, GeoDataCreator* gdcreater, SolverDefinitionGridAttribute* condition) :
GeoDataPolyDataGroup(d, gdcreater, condition),
	impl {new Impl {this}}
{
	addAction()->setText(tr("&Add New Line..."));

	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		auto mapper = impl->m_edgesActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPolyLinesEdgesActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPolyLinesPointsActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}

	actorCollection()->AddItem(impl->m_edgesActor);

	renderer()->AddActor(impl->m_edgesActor);
	renderer()->AddActor(impl->m_selectedPolyLinesEdgesActor);
	renderer()->AddActor(impl->m_selectedPolyLinesPointsActor);

	auto att = gridAttribute();
	if (att && att->isReferenceInformation()) {
		impl->m_colorSetting.mapping = GeoDataPolyLineGroupColorSettingDialog::Arbitrary;
	}

	updateActorSetting();
}

GeoDataPolyLineGroup::~GeoDataPolyLineGroup()
{
	actorCollection()->RemoveAllItems();
	renderer()->RemoveActor(impl->m_edgesActor);
	renderer()->RemoveActor(impl->m_selectedPolyLinesEdgesActor);
	renderer()->RemoveActor(impl->m_selectedPolyLinesPointsActor);

	delete impl;
}

QColor GeoDataPolyLineGroup::color() const
{
	return impl->m_colorSetting.color.value();
}

std::vector<GeoDataPolyLineGroupPolyLine*> GeoDataPolyLineGroup::polyLinesInBoundingBox(double xmin, double xmax, double ymin, double ymax) const
{
	auto env = new geos::geom::Envelope(xmin, xmax, ymin, ymax);
	std::vector<void*> qret;
	qTree()->query(env, qret);
	delete env;

	std::vector<GeoDataPolyLineGroupPolyLine*> ret;
	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolyLineGroupPolyLine*> (vptr);
		if (geom == nullptr) {continue;}
		double gxmin, gxmax, gymin, gymax;
		geom->getBoundingRect(&gxmin, &gxmax, &gymin, &gymax);
		if (gxmax < xmin) {continue;}
		if (gxmin > xmax) {continue;}
		if (gymax < ymin) {continue;}
		if (gymin > ymax) {continue;}

		ret.push_back(geom);
	}
	auto etd = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (editTargetData());
	if (etd != nullptr) {
		double gxmin, gxmax, gymin, gymax;
		etd->getBoundingRect(&gxmin, &gxmax, &gymin, &gymax);
		bool ok = true;
		if (gxmax < xmin) {ok = false;}
		if (gxmin > xmax) {ok = false;}
		if (gymax < ymin) {ok = false;}
		if (gymin > ymax) {ok = false;}
		if (ok) {
			ret.push_back(etd);
		}
	}
	return ret;
}

void GeoDataPolyLineGroup::updateVtkObjects()
{
	impl->m_points->Initialize();
	impl->m_edgesPolyData->Initialize();

	auto edges = vtkSmartPointer<vtkCellArray>::New();

	auto edgeValues = vtkSmartPointer<vtkDoubleArray>::New();
	edgeValues->SetName(VALUE.c_str());

	vtkIdType offset = 0;
	for (auto it = data().rbegin(); it != data().rend(); ++it) {
		auto line = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (*it);
		double v = line->value().toDouble();
		for (const QPointF& p : line->points()) {
			impl->m_points->InsertNextPoint(p.x(), p.y(), 0);
		}
		auto lineEdges = line->lineEdges();
		vtkIdType pts[2];
		for (int i = 0; i < lineEdges.size() / 2; ++i) {
			pts[0] = lineEdges.at(i * 2) + offset;
			pts[1] = lineEdges.at(i * 2 + 1) + offset;
			edges->InsertNextCell(2, pts);
			edgeValues->InsertNextValue(v);
		}
		line->setIndexOffset(offset);
		offset = impl->m_points->GetNumberOfPoints();
	}
	impl->m_points->Modified();
	impl->m_edgesPolyData->SetPoints(impl->m_points);
	impl->m_edgesPolyData->SetLines(edges);
	impl->m_edgesPolyData->GetCellData()->AddArray(edgeValues);
	impl->m_edgesPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPolyLineGroup::updateSelectedDataVtkObjects()
{
	impl->m_selectedPolyLinesEdgesPolyData->Initialize();
	impl->m_selectedPolyLinesPointsPolyData->Initialize();

	auto edges = vtkSmartPointer<vtkCellArray>::New();
	auto points = vtkSmartPointer<vtkCellArray>::New();

	auto edgeValues = vtkSmartPointer<vtkDoubleArray>::New();
	edgeValues->SetName(VALUE.c_str());
	auto pointValues = vtkSmartPointer<vtkDoubleArray>::New();
	pointValues->SetName(VALUE.c_str());

	for (auto d : selectedData()) {
		auto line = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (d);
		double v = line->value().toDouble();
		auto offset = line->indexOffset();
		vtkIdType pts[2];
		unsigned int idx = 0;
		for (const auto& p : line->points()) {
			pts[0] = offset + idx;
			points->InsertNextCell(1, &(pts[0]));
			pointValues->InsertNextValue(v);
			++ idx;
		}
		auto lineEdges = line->lineEdges();
		for (int i = 0; i < lineEdges.size() / 2; ++i) {
			pts[0] = lineEdges.at(i * 2) + offset;
			pts[1] = lineEdges.at(i * 2 + 1) + offset;
			edges->InsertNextCell(2, pts);
			edgeValues->InsertNextValue(v);
		}
	}
	impl->m_selectedPolyLinesPointsPolyData->SetPoints(impl->m_points);
	impl->m_selectedPolyLinesPointsPolyData->SetVerts(points);
	impl->m_selectedPolyLinesPointsPolyData->GetCellData()->AddArray(pointValues);
	impl->m_selectedPolyLinesPointsPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());

	impl->m_selectedPolyLinesEdgesPolyData->SetPoints(impl->m_points);
	impl->m_selectedPolyLinesEdgesPolyData->SetLines(edges);
	impl->m_selectedPolyLinesEdgesPolyData->GetCellData()->AddArray(edgeValues);
	impl->m_selectedPolyLinesEdgesPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPolyLineGroup::selectDataWithPoint(const QPointF& point)
{
	auto& selData = selectedData();
	selData.clear();

	auto v = graphicsView();
	double r = v->stdRadius(iRIC::nearRadius());
	QPolygonF pol;
	pol.push_back(point + QPointF(-r, -r));
	pol.push_back(point + QPointF( r, -r));
	pol.push_back(point + QPointF( r,  r));
	pol.push_back(point + QPointF(-r,  r));
	pol.push_back(point + QPointF(-r, -r));

	auto geosPol = PolygonUtil::getGeosPolygon(pol);
	auto env = geosPol->getEnvelopeInternal();
	std::vector<void*> qret;
	qTree()->query(env, qret);

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolyLineGroupPolyLine*> (vptr);
		if (geom == nullptr) {continue;}
		if (geosPol->intersects(geom->getGeosLineString())) {
			selData.insert(geom);
		}
	}
	delete geosPol;
}

void GeoDataPolyLineGroup::selectDataWithPolygon(const QPolygonF& polygon)
{
	auto& selData = selectedData();
	selData.clear();

	auto geosPol = PolygonUtil::getGeosPolygon(polygon);
	auto env = geosPol->getEnvelopeInternal();
	std::vector<void*> qret;
	qTree()->query(env, qret);

	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolyLineGroupPolyLine*> (vptr);
		if (geom == nullptr) {continue;}
		if (geosPol->contains(geom->getGeosLineString())) {
			selData.insert(geom);
		}
	}
	delete geosPol;
}

std::vector<vtkActor*> GeoDataPolyLineGroup::actorsToShowWhenSelected()
{
	std::vector<vtkActor*> ret;
	ret.push_back(impl->m_selectedPolyLinesEdgesActor);
	ret.push_back(impl->m_selectedPolyLinesPointsActor);
	return ret;
}

bool GeoDataPolyLineGroup::isMergablePolyData(GeoData* geoData)
{
	return dynamic_cast<GeoDataPolyLine*> (geoData) != nullptr;
}

bool GeoDataPolyLineGroup::isMergablePolyDataGroup(GeoData* geoData)
{
	return dynamic_cast<GeoDataPolyLineGroup*> (geoData) != nullptr;
}

QString GeoDataPolyLineGroup::captionForData(int number)
{
	return tr("Line%1").arg(number);
}

GeoDataPolyDataGroup* GeoDataPolyLineGroup::createInstanceForCopy(PreProcessorGeoDataDataItemInterface* d)
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(d->parent());
	return dynamic_cast<GeoDataPolyDataGroup*> (getPolyLineGroupCreator(d, gItem->condition())->create(d, gItem->condition()));
}

void GeoDataPolyLineGroup::setupMenu()
{
	m_menu->setTitle(tr("&Lines"));

	updateMenu();
}

void GeoDataPolyLineGroup::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolyLineGroup::assignActorZValues(const ZDepthRange& range)
{
	GeoDataPolyDataGroup::assignActorZValues(range);

	impl->m_edgesActor->SetPosition(0, 0, range.max());
	impl->m_selectedPolyLinesEdgesActor->SetPosition(0, 0, range.max());
	impl->m_selectedPolyLinesPointsActor->SetPosition(0, 0, range.max());
}

QDialog* GeoDataPolyLineGroup::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolyLineGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_colorSetting);
	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		dialog->setIsReferenceInformation(gridAtt->isReferenceInformation());
	}
	return dialog;
}

void GeoDataPolyLineGroup::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GeoDataPolyLineGroupColorSettingDialog*> (d);
	pushRenderCommand(new SetColorSettingCommand(dialog->setting(), this));
}

GeoDataProxy* GeoDataPolyLineGroup::getProxy()
{
	return new GeoDataPolyLineGroupProxy(this);
}

void GeoDataPolyLineGroup::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_colorSetting.load(node);
}

void GeoDataPolyLineGroup::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_colorSetting.save(writer);
}

GeoDataPolyDataGroupPolyData* GeoDataPolyLineGroup::createNewData()
{
	return new GeoDataPolyLineGroupPolyLine(this);
}

GeoDataPolyData* GeoDataPolyLineGroup::createEditTargetData()
{
	auto line = new GeoDataPolyLine(parent(), creator(), gridAttribute());

	line->assignActorZValues(depthRange());
	connect(line, SIGNAL(nameAndValueEdited()), this, SLOT(updateAttributeBrowser()));

	return line;
}

void GeoDataPolyLineGroup::updateActorSetting()
{
	auto cs = impl->m_colorSetting;

	// color
	QColor c = cs.color;

	impl->m_edgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_selectedPolyLinesEdgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_selectedPolyLinesPointsActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_edgesActor->GetProperty()->SetOpacity(cs.opacity);

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPolyLineGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	impl->m_edgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_selectedPolyLinesEdgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_selectedPolyLinesPointsActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_selectedPolyLinesEdgesActor->GetProperty()->SetOpacity(cs.opacity);

	// line width and point sizes
	impl->m_edgesActor->GetProperty()->SetLineWidth(cs.lineWidth);
	impl->m_selectedPolyLinesPointsActor->GetProperty()->SetPointSize(cs.lineWidth * 5);

	updateActorSettingForEditTargetPolyData();
}

void GeoDataPolyLineGroup::updateMenu()
{
	auto att = gridAttribute();
	bool isRef = att && att->isReferenceInformation();
	GeoDataPolyLine* p = impl->m_dummyPolyLineForMenu;
	if (editTargetData() != nullptr) {
		p = dynamic_cast<GeoDataPolyLine*> (editTargetData());
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
	auto sortMenu = m->addMenu(tr("&Sort"));
	sortMenu->addAction(moveToTopAction());
	sortMenu->addAction(moveUpAction());
	sortMenu->addAction(moveDownAction());
	sortMenu->addAction(moveToBottomAction());

	m->addSeparator();
	m->addAction(mergeAction());
	m->addAction(copyAction());
	m->addAction(editDisplaySettingAction());
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
	sortMenu = m->addMenu(tr("&Sort"));
	sortMenu->addAction(moveToTopAction());
	sortMenu->addAction(moveUpAction());
	sortMenu->addAction(moveDownAction());
	sortMenu->addAction(moveToBottomAction());

	m->addSeparator();
	m->addAction(mergeAction());
	m->addAction(copyAction());
	m->addAction(editDisplaySettingAction());
	m->addAction(attributeBrowserAction());

	m->addSeparator();
	m->addAction(deleteAction());
}

void GeoDataPolyLineGroup::updateActorSettingForEditTargetPolyData()
{
	auto t = editTargetData();
	if (t == nullptr) {return;}

	auto targetData = dynamic_cast<GeoDataPolyLine*> (t);

	const auto& cs = impl->m_colorSetting;
	targetData->setColor(cs.color);
	targetData->setOpacity(cs.opacity);
	if (cs.mapping == GeoDataPolyLineGroupColorSettingDialog::Arbitrary) {
		targetData->setMapping(GeoDataPolyDataColorSettingDialog::Arbitrary);
	} else {
		targetData->setMapping(GeoDataPolyDataColorSettingDialog::Value);
	}
	targetData->setLineWidth(cs.lineWidth);
}
