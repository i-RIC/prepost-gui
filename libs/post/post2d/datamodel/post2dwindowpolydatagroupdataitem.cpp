#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowpolydatagroupdataitem.h"
#include "post2dwindowpolydatavaluedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowpolydatagroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/stringtool.h>

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowPolyDataGroupDataItem::Post2dWindowPolyDataGroupDataItem(const std::string& name, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {name.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_setting {}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();

	SolverDefinitionGridType* gt = cont->gridType();

	for (const auto& name : vtkDataSetAttributesTool::getArrayNames(cont->polyData(name)->data()->GetCellData())) {
		auto item = new Post2dWindowPolyDataValueDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
		auto cs = new ColorMapSettingContainer();
		auto caption = gt->output(name)->caption();
		cs->valueCaption = caption;
		cs->legend.title = caption;
		m_colorMapSettings.insert({name, cs});
	}

	setupActors();
	updateActorSettings();
}

Post2dWindowPolyDataGroupDataItem::~Post2dWindowPolyDataGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();;
	m_legendActor->Delete();

	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}
}

std::string Post2dWindowPolyDataGroupDataItem::name() const
{
	return iRIC::toStr(m_standardItem->text());
}

std::string Post2dWindowPolyDataGroupDataItem::target() const
{
	return std::string(m_setting.value);
}

void Post2dWindowPolyDataGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.value = target.c_str();
	if (m_setting.value == "") {
		m_setting.mapping = PolyDataSetting::Mapping::Arbitrary;
	} else {
		m_setting.mapping = PolyDataSetting::Mapping::Value;
	}

	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::update()
{
	auto gtItem = zoneDataItem()->gridTypeDataItem();
	for (const auto& pair : m_colorMapSettings) {
		auto range = gtItem->polyDataValueRange(pair.first);
		pair.second->setAutoValueRange(range);
	}

	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_legendActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	auto data = polyData();
	if (data == nullptr) {return;}

	if (m_setting.mapping == PolyDataSetting::Mapping::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		m_actor->SetMapper(mapper);
		mapper->Delete();;

		m_actor->GetProperty()->SetColor(m_setting.color);
	} else {
		auto value = iRIC::toStr(m_setting.value);
		data->GetCellData()->SetActiveScalars(value.c_str());
		auto cs = m_colorMapSettings.at(value);
		auto mapper = cs->buildCellDataMapper(data, false);
		m_actor->SetMapper(mapper);
		mapper->Delete();

		auto& is = cs->legend.imageSetting;
		is.setActor(m_legendActor);
		is.controller()->setItem(this);
		is.controller()->handleSelection(dataModel()->graphicsView());
	}
	auto v = dataModel()->graphicsView();
	m_actor->GetProperty()->SetLineWidth(m_setting.lineWidth * v->devicePixelRatioF());
	m_actor->GetProperty()->SetOpacity(m_setting.opacity);
	m_actorCollection->AddItem(m_actor);

	updateCheckState();
	updateVisibilityWithoutRendering();
}

void Post2dWindowPolyDataGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowPolyDataGroupDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

void Post2dWindowPolyDataGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowPolyDataGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowPolyDataGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleSelection(v);
	}

	zoneDataItem()->initPolyDataResultAttributeBrowser();
}

void Post2dWindowPolyDataGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleDeselection(v);
	}

	zoneDataItem()->clearPolyDataResultAttributeBrowser();
}

void Post2dWindowPolyDataGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleResize(event, v);
	}
}

void Post2dWindowPolyDataGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	}

	zoneDataItem()->updatePolyDataResultAttributeBrowser(name(), event->pos(), v);
}

void Post2dWindowPolyDataGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleMousePressEvent(event, v);
	}
}

void Post2dWindowPolyDataGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	}

	zoneDataItem()->fixPolyDataResultAttributeBrowser(name(), event->pos(), v);
}

void Post2dWindowPolyDataGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* a = zoneDataItem()->showAttributeBrowserActionForPolyDataResult();
	menu->addAction(a);
}

void Post2dWindowPolyDataGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("PolyData value change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowPolyDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		if (child.nodeName() == "ColorMapSetting") {
			auto elem = child.toElement();
			auto name = iRIC::toStr(elem.attribute("name"));
			auto it = m_colorMapSettings.find(name);
			if (it == m_colorMapSettings.end()) {continue;}

			it->second->load(child);
		}
	}

	updateCheckState();
	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (const auto& pair : m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

ColorMapSettingContainer* Post2dWindowPolyDataGroupDataItem::activeSetting() const
{
	if (m_setting.mapping == PolyDataSetting::Mapping::Arbitrary) {return nullptr;}

	return m_colorMapSettings.at(iRIC::toStr(m_setting.value));
}

Post2dWindowGridTypeDataItem* Post2dWindowPolyDataGroupDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dWindowGridTypeDataItem*> (zoneDataItem()->parent());
}

Post2dWindowZoneDataItem* Post2dWindowPolyDataGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent());
}

vtkPolyData* Post2dWindowPolyDataGroupDataItem::polyData() const
{
	auto cont = zoneDataItem()->dataContainer();
	if (cont == nullptr) {return nullptr;}

	return cont->polyData(name())->concreteData();
}

void Post2dWindowPolyDataGroupDataItem::updateCheckState()
{
	if (m_setting.mapping == PolyDataSetting::Mapping::Arbitrary) {
		NamedGraphicsWindowDataItemTool::checkItemWithName("", m_childItems, true);
	} else if (m_setting.mapping == PolyDataSetting::Mapping::Value) {
		NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.value, m_childItems, true);
	}
}
