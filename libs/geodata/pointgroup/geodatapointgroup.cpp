#include "geodatapointgroup.h"
#include "geodatapointgroupcolorsettingdialog.h"
#include "geodatapointgroupcreator.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupproxy.h"
#include "private/geodatapointgroup_impl.h"
#include "private/geodatapointgroup_setcolorsettingcommand.h"

#include <geodata/point/geodatapoint.h>
#include <geodata/polydatagroup/geodatapolydatagroupcreator.h>
#include <geoio/polygonutil.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/geodata/geodatafactoryinterface.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
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

GeoDataCreator* getPointGroupCreator(PreProcessorGeoDataDataItemInterface* geoData, SolverDefinitionGridAttribute* att)
{
	auto factory = geoData->preProcessorWindow()->geoDataFactory();
	auto creators = factory->compatibleCreators(att);
	for (auto c : creators) {
		auto c2 = dynamic_cast <GeoDataPointGroupCreator*> (c);
		if (c2 != nullptr) {return c2;}
	}
	return nullptr;
}

} // namespace

GeoDataPointGroup::GeoDataPointGroup(ProjectDataItem* d, GeoDataCreator* gdcreater, SolverDefinitionGridAttribute* condition) :
GeoDataPolyDataGroup(d, gdcreater, condition),
	impl {new Impl {this}}
{
	addAction()->setText(tr("&Add New Point..."));

	actorCollection()->AddItem(impl->m_pointsActor);

	renderer()->AddActor(impl->m_pointsActor);
	renderer()->AddActor(impl->m_selectedPointsPointsActor);

	auto att = gridAttribute();
	if (att && att->isReferenceInformation()) {
		impl->m_colorSetting.mapping = GeoDataPointGroupColorSettingDialog::Arbitrary;
	}

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
	impl->m_vtkPoints->Modified();
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

GeoDataPolyDataGroup* GeoDataPointGroup::createInstanceForCopy(PreProcessorGeoDataDataItemInterface* d)
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (d->parent());
	return dynamic_cast<GeoDataPolyDataGroup*> (getPointGroupCreator(d, gItem->condition())->create(d, gItem->condition()));
}

void GeoDataPointGroup::setupMenu()
{
	m_menu->setTitle(tr("P&oint"));

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


QDialog* GeoDataPointGroup::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPointGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_colorSetting);
	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		dialog->setIsReferenceInformation(gridAtt->isReferenceInformation());
	}
	return dialog;
}

void GeoDataPointGroup::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GeoDataPointGroupColorSettingDialog*> (d);
	pushRenderCommand(new SetColorSettingCommand(dialog->setting(), this));
}

GeoDataProxy* GeoDataPointGroup::getProxy()
{
	return new GeoDataPointGroupProxy(this);
}

void GeoDataPointGroup::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_colorSetting.load(node);
}

void GeoDataPointGroup::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_colorSetting.save(writer);
}

GeoDataPolyDataGroupPolyData* GeoDataPointGroup::createNewData()
{
	return new GeoDataPointGroupPoint(this);
}

GeoDataPolyData* GeoDataPointGroup::createEditTargetData()
{
	auto point = new GeoDataPoint(parent(), creator(), gridAttribute());
	point->setVariantValue(gridAttribute()->variantDefaultValue());

	point->assignActorZValues(depthRange());
	connect(point, SIGNAL(nameAndValueEdited()), this, SLOT(updateAttributeBrowser()));

	return point;
}

void GeoDataPointGroup::updateActorSetting()
{
	auto cs = impl->m_colorSetting;

	// color
	QColor c = cs.color;

	impl->m_pointsActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_selectedPointsPointsActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPointGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	if (scalarVisibility) {
		vtkMapper* mapper = nullptr;
		auto cs = colorMapSettingContainer();

		mapper = cs->buildCellDataMapper(impl->m_pointsPolyData, true);
		impl->m_pointsActor->SetMapper(mapper);
		mapper->Delete();

		mapper = cs->buildCellDataMapper(impl->m_selectedPointsPointsPolyData, true);
		impl->m_selectedPointsPointsActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_pointsPolyData);
		impl->m_pointsActor->SetMapper(mapper);
		mapper->Delete();

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_selectedPointsPointsPolyData);
		impl->m_selectedPointsPointsActor->SetMapper(mapper);
		mapper->Delete();
	}

	// opacity
	impl->m_pointsActor->GetProperty()->SetOpacity(cs.opacity);
	impl->m_selectedPointsPointsActor->GetProperty()->SetOpacity(cs.opacity);

	// pointSize
	impl->m_pointsActor->GetProperty()->SetPointSize(cs.pointSize);
	impl->m_selectedPointsPointsActor->GetProperty()->SetPointSize(cs.pointSize * 2);

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

void GeoDataPointGroup::updateActorSettingForEditTargetPolyData()
{
	auto t = editTargetData();
	if (t == nullptr) {return;}

	auto targetData = dynamic_cast<GeoDataPoint*> (t);

	const auto& cs = impl->m_colorSetting;
	targetData->setColor(cs.color);
	targetData->setOpacity(cs.opacity);
	if (cs.mapping == GeoDataPointGroupColorSettingDialog::Arbitrary) {
		targetData->setMapping(GeoDataPolyDataColorSettingDialog::Arbitrary);
	} else {
		targetData->setMapping(GeoDataPolyDataColorSettingDialog::Value);
	}
	targetData->setPointSize(cs.pointSize * 2);
}
