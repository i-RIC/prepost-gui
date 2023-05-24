#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/boolcontainerwidget.h>
#include <guicore/arrows/arrowssettingtoolbarwidget.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

#include <vtkActor.h>
#include <vtkActor2D.h>

Post2dWindowNodeVectorArrowGroupDataItem::Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_actor {vtkActor::New()},
	m_arrowLegendActor {vtkActor2D::New()},
	m_colorLegendActor {vtkActor2D::New()},
	m_arrowsToolBarWidget {new ArrowsSettingToolBarWidget(mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = zoneDataItem()->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->data()->GetPointData())) {
		auto item = new Post2dWindowNodeVectorArrowDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}

	for (const auto& pair : cont->data()->valueRangeSet().pointDataValueRanges()) {
		auto caption = gt->output(pair.first)->caption();
		createOrUpdateColorMapsSetting(pair.first, caption, pair.second);
	}

	m_arrowsToolBarWidget->hide();
	m_arrowsToolBarWidget->setColorMapSettings(m_colorMapSettings);
	m_arrowsToolBarWidget->setSetting(&m_setting);

	connect(m_arrowsToolBarWidget, &ArrowsSettingToolBarWidget::updated, [=]() {
		auto com = new MergeSupportedListCommand(iRIC::generateCommandId("ArrowModify"), false);
		auto newSetting = m_arrowsToolBarWidget->modifiedSetting();
		com->addCommand(new ValueModifyCommmand<ArrowsSettingContainer>(iRIC::generateCommandId("ArrowsSetting"), false, newSetting, &m_setting));

		if (newSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
			auto cm = m_colorMapSettings.at(iRIC::toStr(newSetting.colorTarget));
			com->addCommand(new ValueModifyCommmand<ColorMapSettingContainer>(iRIC::generateCommandId("ColorMapSetting"), false, m_arrowsToolBarWidget->modifiedColorMapSetting(), cm));
		}
		pushUpdateActorSettingCommand(com, this);
	});

	setupActors();
}

Post2dWindowNodeVectorArrowGroupDataItem::~Post2dWindowNodeVectorArrowGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_arrowLegendActor);
	r->RemoveActor2D(m_colorLegendActor);

	m_actor->Delete();
	m_arrowLegendActor->Delete();
	m_colorLegendActor->Delete();

	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto newSetting = m_setting;
	if (item->standardItem()->checkState() == Qt::Checked) {
		newSetting.target = item->name().c_str();
		newSetting.legend.title = zoneDataItem()->gridTypeDataItem()->gridType()->output(item->name())->caption();
	} else {
		newSetting.target = "";
	}
	auto modifyCommand = new ValueModifyCommmand<ArrowsSettingContainer> ("Change target", newSetting, &m_setting);
	auto updateCommand = new UpdateActorSettingCommand(false, modifyCommand, this);
	pushRenderCommand(updateCommand, this, true);
}

void Post2dWindowNodeVectorArrowGroupDataItem::setupActors()
{
	vtkRenderer* r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_arrowLegendActor);
	r->AddActor2D(m_colorLegendActor);

	m_actor->GetProperty()->SetLighting(false);

	m_setting.legend.imageSetting.setActor(m_arrowLegendActor);
	m_setting.legend.imageSetting.controller()->setItem(this);
}

void Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
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
}

void Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (const auto& pair : m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateActorSetting()
{
	m_actor->VisibilityOff();
	m_arrowLegendActor->VisibilityOff();
	m_colorLegendActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	if (m_setting.target == "") {return;}

	auto data = buildFilteredData();
	if (data == nullptr) {return;}

	auto view = dataModel()->graphicsView();
	auto arrowsData = m_setting.buildArrowsPolygonData(data, view);
	data->Delete();
	auto v = dataModel()->graphicsView();

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetColor(m_setting.customColor);
	} else if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		arrowsData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.colorTarget).c_str());
		auto colorTarget = iRIC::toStr(m_setting.colorTarget);
		auto cs = colorMapSetting(colorTarget);
		auto mapper = cs->buildPointDataMapper(arrowsData);
		m_actor->SetMapper(mapper);
		mapper->Delete();
		cs->legend.imageSetting.setActor(m_colorLegendActor);
		cs->legend.imageSetting.controller()->setItem(this);
		cs->legend.imageSetting.controller()->handleSelection(v);
	}
	arrowsData->Delete();

	m_actor->GetProperty()->SetLineWidth(m_setting.lineWidth * v->devicePixelRatioF());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();

	m_setting.legend.imageSetting.apply(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.target, m_childItems, true);
}

Post2dWindowZoneDataItem* Post2dWindowNodeVectorArrowGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

void Post2dWindowNodeVectorArrowGroupDataItem::createOrUpdateColorMapsSetting(const std::string& name, const QString& caption, const ValueRangeContainer& range)
{
	ColorMapSettingContainer* setting = nullptr;
	auto it = m_colorMapSettings.find(name);
	if (it == m_colorMapSettings.end()) {
		setting = new ColorMapSettingContainer();
		setting->valueCaption = caption;
		setting->legend.title = caption;
		m_colorMapSettings.insert({name, setting});
	} else {
		setting = it->second;
	}
	setting->setAutoValueRange(range);
}

ColorMapSettingContainer* Post2dWindowNodeVectorArrowGroupDataItem::colorMapSetting(const std::string& name) const
{
	return m_colorMapSettings.at(name);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeVectorArrowGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.max());
}

void Post2dWindowNodeVectorArrowGroupDataItem::update()
{
	auto gtItem = zoneDataItem()->gridTypeDataItem();
	for (const auto& pair : m_colorMapSettings) {
		auto range = gtItem->nodeValueRange(pair.first);
		pair.second->setAutoValueRange(range);
	}

	updateActorSetting();
}

std::string Post2dWindowNodeVectorArrowGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void Post2dWindowNodeVectorArrowGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();

	updateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupDataItem::innerUpdate2Ds()
{
	updateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupDataItem::informSelection(VTKGraphicsView* v)
{
	zoneDataItem()->initNodeResultAttributeBrowser();

	m_setting.legend.imageSetting.controller()->handleSelection(v);

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}
	auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleSelection(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();

	m_setting.legend.imageSetting.controller()->handleDeselection(v);

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}
	auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleDeselection(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	m_setting.legend.imageSetting.controller()->handleResize(event, v);

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}
	auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleResize(event, v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateNodeResultAttributeBrowser(event->pos(), v);
	if (m_setting.target == "") {return;}

	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
		if (s != nullptr) {
			s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
			controllers.push_back(s->legend.imageSetting.controller());
		}
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (m_setting.target == "") {return;}

	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
		if (s != nullptr) {
			s->legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
			controllers.push_back(s->legend.imageSetting.controller());
		}
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixNodeResultAttributeBrowser(event->pos(), v);
	if (m_setting.target == "") {return;}

	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
		if (s != nullptr) {
			s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
			controllers.push_back(s->legend.imageSetting.controller());
		}
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowNodeVectorArrowGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = zoneDataItem()->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}

bool Post2dWindowNodeVectorArrowGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_arrowsToolBarWidget->setParent(toolBar);
	m_arrowsToolBarWidget->show();

	toolBar->addWidget(m_arrowsToolBarWidget);

	return true;
}
