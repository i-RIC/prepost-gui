#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalardataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesbasescalargroupdataitem_settingeditwidget.h"
#include "private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidget.h"
#include "private/post2dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

Post2dWindowParticlesBaseScalarGroupDataItem::Post2dWindowParticlesBaseScalarGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p),
	m_actor {vtkActor::New()},
	m_setting {},
	m_toolBarWidget {new ToolBarWidget(this, mainWindow())},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(mainWindow())},
	m_toolBarWidgetController {new ToolBarWidgetController(m_colorMapToolBarWidget, this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto topItem = topDataItem();
	auto cont = topItem->zoneDataItem()->dataContainer();
	auto gt = cont->gridType();

	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(topItem->particleData()->GetPointData())){
		auto item = new Post2dWindowParticlesBaseScalarDataItem(name, gt->outputCaption(name), this);
		m_childItems.push_back(item);
	}
	m_toolBarWidget->hide();
	m_colorMapToolBarWidget->hide();

	setupActors();
	updateCheckState();
}

Post2dWindowParticlesBaseScalarGroupDataItem::~Post2dWindowParticlesBaseScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);

	m_actor->Delete();

	delete m_toolBarWidgetController;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::update()
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseScalarDataItem*> (child);
		item->update();
	}

	updateActorSetting();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowParticlesBaseScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Particles Scalar Setting"));
	dialog->resize(900, 650);

	return dialog;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

std::string Post2dWindowParticlesBaseScalarGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.value);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems, true);
	m_setting.value = target.c_str();
	if (m_setting.value == "") {
		m_setting.mapping = ParticleDataSetting::Mapping::Arbitrary;
	} else {
		m_setting.mapping = ParticleDataSetting::Mapping::Value;
	}
	updateActorSetting();
	emit m_setting.updated();
}

ColorMapSettingContainerI* Post2dWindowParticlesBaseScalarGroupDataItem::colorMapSetting(const std::string& name) const
{
	auto child = childDataItem(name);
	if (child == nullptr) {return nullptr;}

	return child->colorMapSetting();
}

std::unordered_map<std::string, ColorMapSettingContainerI*> Post2dWindowParticlesBaseScalarGroupDataItem::colorMapSettings() const
{
	std::unordered_map<std::string, ColorMapSettingContainerI*> ret;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseScalarDataItem*> (child);
		auto cm = item->colorMapSetting();
		ret.insert({item->name(), cm});
	}

	return ret;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->initParticleResultAttributeBrowser(particleData());
}

void Post2dWindowParticlesBaseScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

void Post2dWindowParticlesBaseScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(this, event, v);
	}

	zoneDataItem()->updateParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(this, event, v);
	}
}

void Post2dWindowParticlesBaseScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(this, event, v);
	}

	zoneDataItem()->fixParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	menu->addAction(topItem->showAttributeBrowserAction());
}

bool Post2dWindowParticlesBaseScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_toolBarWidget->setParent(toolBar);
	m_toolBarWidget->show();
	toolBar->addWidget(m_toolBarWidget);
	toolBar->addSeparator();

	m_colorMapToolBarWidget->setParent(toolBar);
	m_colorMapToolBarWidget->show();
	toolBar->addWidget(m_colorMapToolBarWidget);

	return true;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	topDataItem()->updateColorMapLegendsVisibility();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Scalar value change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);

	update();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::updateActorSetting()
{
	updateCheckState();

	m_colorMapToolBarWidget->setDisabled(true);
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
			auto cs = activeColorMapSetting();
			auto mapper = cs->buildPointDataMapper(data);
			m_actor->SetMapper(mapper);
			mapper->Delete();

			m_colorMapToolBarWidget->setEnabled(true);
			m_colorMapToolBarWidget->setSetting(cs);
		}
	}
	m_actor->GetProperty()->SetPointSize(m_setting.particleSize * v->devicePixelRatioF());
	m_actor->GetProperty()->SetOpacity(m_setting.opacity);
	m_actorCollection->AddItem(m_actor);

	topDataItem()->updateColorMapLegendsVisibility();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	std::unordered_map<std::string, Post2dWindowParticlesBaseScalarDataItem*> itemMap;
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowParticlesBaseScalarDataItem*> (item);
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

	updateActorSetting();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowParticlesBaseScalarDataItem*> (item);
		writer.writeStartElement("Scalar");
		writer.writeAttribute("name", item2->name().c_str());
		item2->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

ColorMapSettingContainerI* Post2dWindowParticlesBaseScalarGroupDataItem::activeColorMapSetting() const
{
	if (m_standardItem->checkState() == Qt::Unchecked) {return nullptr;}
	if (m_setting.mapping ==  ParticleDataSetting::Mapping::Arbitrary) {return nullptr;}
	if (m_setting.value == "") {return nullptr;}

	return activeChildDataItem()->colorMapSetting();
}

ColorMapSettingContainerI* Post2dWindowParticlesBaseScalarGroupDataItem::activeColorMapSettingWithVisibleLegend() const
{
	auto cm = activeColorMapSetting();
	if (cm == nullptr) {return nullptr;}
	if (! cm->legendSetting()->getVisible()) {return nullptr;}

	return cm;
}

Post2dWindowGridTypeDataItem* Post2dWindowParticlesBaseScalarGroupDataItem::gridTypeDataItem() const
{
	return zoneDataItem()->gridTypeDataItem();
}

Post2dWindowParticlesBaseTopDataItem* Post2dWindowParticlesBaseScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowParticlesBaseScalarGroupDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

Post2dWindowParticlesBaseScalarDataItem* Post2dWindowParticlesBaseScalarGroupDataItem::activeChildDataItem() const
{
	if (m_setting.value == "") {return nullptr;}

	return childDataItem(iRIC::toStr(m_setting.value));
}

Post2dWindowParticlesBaseScalarDataItem* Post2dWindowParticlesBaseScalarGroupDataItem::childDataItem(const std::string& name) const
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseScalarDataItem*> (child);
		if (name == item->name()) {
			return item;
		}
	}

	return nullptr;
}

vtkPolyData* Post2dWindowParticlesBaseScalarGroupDataItem::particleData() const
{
	return topDataItem()->particleData();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::updateCheckState()
{
	if (m_setting.mapping == ParticleDataSetting::Mapping::Arbitrary) {
		NamedGraphicsWindowDataItemTool::checkItemWithName("", m_childItems, true);
	} else if (m_setting.mapping == ParticleDataSetting::Mapping::Value) {
		NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.value, m_childItems, true);
	}
}
