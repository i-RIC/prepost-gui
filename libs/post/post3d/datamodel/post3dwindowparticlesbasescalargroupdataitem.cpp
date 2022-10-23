#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesbasescalardataitem.h"
#include "post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowparticlesbasescalargroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

Post3dWindowParticlesBaseScalarGroupDataItem::Post3dWindowParticlesBaseScalarGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p),
	m_actor {vtkActor::New()},
	m_setting {}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto topItem = topDataItem();
	auto cont = topItem->zoneDataItem()->dataContainer();
	auto gt = cont->gridType();

	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(topItem->particleData()->GetPointData())){
		auto item = new Post3dWindowParticlesBaseScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}

	setupActors();
	updateCheckState();
}

Post3dWindowParticlesBaseScalarGroupDataItem::~Post3dWindowParticlesBaseScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);

	m_actor->Delete();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::update()
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseScalarDataItem*> (child);
		item->update();
	}

	updateActorSettings();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post3dWindowParticlesBaseScalarGroupDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

std::string Post3dWindowParticlesBaseScalarGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.value);
}

void Post3dWindowParticlesBaseScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.value = target.c_str();
	if (m_setting.value == "") {
		m_setting.mapping = ParticleDataSetting::Mapping::Arbitrary;
	} else {
		m_setting.mapping = ParticleDataSetting::Mapping::Value;
	}
	updateActorSettings();
}

ColorMapSettingContainer* Post3dWindowParticlesBaseScalarGroupDataItem::colorMapSetting(const std::string& name) const
{
	auto child = childDataItem(name);
	if (child == nullptr) {return nullptr;}

	return &child->colorMapSetting();
}

std::unordered_map<std::string, ColorMapSettingContainer*> Post3dWindowParticlesBaseScalarGroupDataItem::colorMapSettings() const
{
	std::unordered_map<std::string, ColorMapSettingContainer*> ret;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseScalarDataItem*> (child);
		auto& cm = item->colorMapSetting();
		ret.insert({item->name(), &cm});
	}

	return ret;
}

void Post3dWindowParticlesBaseScalarGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleSelection(v);
	}
}

void Post3dWindowParticlesBaseScalarGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleDeselection(v);
	}
}

void Post3dWindowParticlesBaseScalarGroupDataItem::handleResize(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleResize(v);
	}
}


void Post3dWindowParticlesBaseScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	}
}

void Post3dWindowParticlesBaseScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleMousePressEvent(event, v);
	}
}

void Post3dWindowParticlesBaseScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	}
}

void Post3dWindowParticlesBaseScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Scalar value change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowParticlesBaseScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);

	update();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();

	m_actorCollection->RemoveAllItems();

	auto data = particleData();
	if (data == nullptr) {return;}

	auto v = dataModel()->graphicsView();
	if (m_setting.mapping == ParticleDataSetting::Mapping::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetColor(m_setting.color);
	} else {
		auto value = iRIC::toStr(m_setting.value);
		if (value != "") {
			data->GetPointData()->SetActiveScalars(value.c_str());
			auto cs = activeSetting();
			auto mapper = cs->buildPointDataMapper(data);
			m_actor->SetMapper(mapper);
			mapper->Delete();

			cs->legend.imageSetting.apply(v);
		}
	}
	m_actor->GetProperty()->SetPointSize(m_setting.particleSize * v->devicePixelRatioF());
	m_actor->GetProperty()->SetOpacity(m_setting.opacity);
	m_actorCollection->AddItem(m_actor);

	updateVisibilityWithoutRendering();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::innerUpdateZScale(double zscale)
{
	m_actor->SetScale(1, 1, zscale);
}

void Post3dWindowParticlesBaseScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	std::unordered_map<std::string, Post3dWindowParticlesBaseScalarDataItem*> itemMap;
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowParticlesBaseScalarDataItem*> (item);
		itemMap.insert({item2->name(), item2});
	}

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "Scalar") {
			auto name = iRIC::toStr(childElem.attribute("name"));
			auto it = itemMap.find(name);
			if (it == itemMap.end()) {continue;}

			it->second->loadFromProjectMainFile(childElem);
		}
	}

	updateCheckState();
	updateActorSettings();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowParticlesBaseScalarDataItem*> (item);
		writer.writeStartElement("Scalar");
		writer.writeAttribute("name", item2->name().c_str());
		item2->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

ColorMapSettingContainer* Post3dWindowParticlesBaseScalarGroupDataItem::activeSetting() const
{
	if (m_setting.mapping ==  ParticleDataSetting::Mapping::Arbitrary) {return nullptr;}
	if (m_setting.value == "") {return nullptr;}

	return &activeChildDataItem()->colorMapSetting();
}

Post3dWindowGridTypeDataItem* Post3dWindowParticlesBaseScalarGroupDataItem::gridTypeDataItem() const
{
	return zoneDataItem()->gridTypeDataItem();
}

Post3dWindowParticlesBaseTopDataItem* Post3dWindowParticlesBaseScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post3dWindowParticlesBaseTopDataItem*> (parent());
}

Post3dWindowZoneDataItem* Post3dWindowParticlesBaseScalarGroupDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

Post3dWindowParticlesBaseScalarDataItem* Post3dWindowParticlesBaseScalarGroupDataItem::activeChildDataItem() const
{
	if (m_setting.value == "") {return nullptr;}

	return childDataItem(iRIC::toStr(m_setting.value));
}

Post3dWindowParticlesBaseScalarDataItem* Post3dWindowParticlesBaseScalarGroupDataItem::childDataItem(const std::string& name) const
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseScalarDataItem*> (child);
		if (name == item->name()) {
			return item;
		}
	}

	return nullptr;
}

vtkPolyData* Post3dWindowParticlesBaseScalarGroupDataItem::particleData() const
{
	return topDataItem()->particleData();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.value, m_childItems, true);
}
