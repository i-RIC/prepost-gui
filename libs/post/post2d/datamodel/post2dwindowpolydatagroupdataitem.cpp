#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowpolydatagroupdataitem.h"
#include "post2dwindowpolydatatopdataitem.h"
#include "post2dwindowpolydatavaluedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowpolydatagroupdataitem_attributebrowsercontroller.h"
#include "private/post2dwindowpolydatagroupdataitem_impl.h"
#include "private/post2dwindowpolydatagroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4polydata2d.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>
#include <postbase/polydata/polydatasettingtoolbarwidget.h>

Post2dWindowPolyDataGroupDataItem::Impl::Impl(Post2dWindowPolyDataGroupDataItem* item) :
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_showAttributeBrowserAction {new QAction {Post2dWindowPolyDataGroupDataItem::tr("Show Attribute Browser"), item}},
	m_attributeBrowserController {new AttributeBrowserController {item}},
	m_setting {},
	m_colorMapSettings {},
	m_polyDataToolBarWidget {new PolyDataSettingToolBarWidget(item->mainWindow())}
{
	connect(m_showAttributeBrowserAction, &QAction::triggered, item, &Post2dWindowPolyDataGroupDataItem::showAttributeBrowser);
}

Post2dWindowPolyDataGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
	m_legendActor->Delete();

	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}
	delete m_attributeBrowserController;
}

Post2dWindowPolyDataGroupDataItem::Post2dWindowPolyDataGroupDataItem(const std::string& name, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {name.c_str(), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();

	SolverDefinitionGridType* gt = cont->gridType();

	for (const auto& name : vtkDataSetAttributesTool::getArrayNames(cont->polyData(name)->grid()->vtkData()->data()->GetCellData())) {
		auto item = new Post2dWindowPolyDataValueDataItem(name, gt->outputCaption(name), this);
		m_childItems.push_back(item);
		auto output = gt->output(name);
		auto cs = output->createColorMapSettingContainer();
		auto caption = output->caption();
		cs->valueCaption = caption;
		cs->legendSetting()->setTitle(caption);
		impl->m_colorMapSettings.insert({name, cs});
	}

	impl->m_polyDataToolBarWidget->hide();
	impl->m_polyDataToolBarWidget->setColorMapSettings(impl->m_colorMapSettings);
	impl->m_polyDataToolBarWidget->setSetting(&impl->m_setting);

	connect(impl->m_polyDataToolBarWidget, &PolyDataSettingToolBarWidget::updated, [=]() {
		auto com = new MergeSupportedListCommand(iRIC::generateCommandId("ArrowModify"), false);
		auto newSetting = impl->m_polyDataToolBarWidget->modifiedSetting();
		com->addCommand(new ValueModifyCommmand<PolyDataSetting>(iRIC::generateCommandId("PolyDataSetting"), false, newSetting, &impl->m_setting));

		if (newSetting.mapping == PolyDataSetting::Mapping::Value) {
			auto cm = impl->m_colorMapSettings.at(iRIC::toStr(newSetting.value));
			com->addCommand(new ColorMapSettingModifyCommand(impl->m_polyDataToolBarWidget->modifiedColorMapSetting(), cm));
		}
		pushUpdateActorSettingCommand(com, this);
	});

	setupActors();
}

Post2dWindowPolyDataGroupDataItem::~Post2dWindowPolyDataGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);
	r->RemoveActor2D(impl->m_legendActor);
}

std::string Post2dWindowPolyDataGroupDataItem::name() const
{
	return iRIC::toStr(m_standardItem->text());
}

std::string Post2dWindowPolyDataGroupDataItem::target() const
{
	return std::string(impl->m_setting.value);
}

void Post2dWindowPolyDataGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_setting.value = target.c_str();
	if (impl->m_setting.value == "") {
		impl->m_setting.mapping = PolyDataSetting::Mapping::Arbitrary;
	} else {
		impl->m_setting.mapping = PolyDataSetting::Mapping::Value;
	}
	emit impl->m_setting.updated();

	updateActorSetting();
}

Post2dWindowAttributeBrowserController* Post2dWindowPolyDataGroupDataItem::attributeBrowserController() const
{
	return impl->m_attributeBrowserController;
}

QAction* Post2dWindowPolyDataGroupDataItem::showAttributeBrowserAction() const
{
	return impl->m_showAttributeBrowserAction;
}

void Post2dWindowPolyDataGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_actor);
	r->AddActor2D(impl->m_legendActor);

	updateActorSetting();
}

void Post2dWindowPolyDataGroupDataItem::update()
{
	auto gtItem = zoneDataItem()->gridTypeDataItem();
	for (const auto& pair : impl->m_colorMapSettings) {
		auto range = gtItem->polyDataValueRange(pair.first);
		pair.second->setAutoValueRange(range);
	}

	updateActorSetting();
}

void Post2dWindowPolyDataGroupDataItem::updateActorSetting()
{
	impl->m_actor->VisibilityOff();
	impl->m_legendActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	if (polyData() == nullptr) {return;}
	auto data = polyData()->vtkConcreteData()->concreteData();

	if (impl->m_setting.mapping == PolyDataSetting::Mapping::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();;

		impl->m_actor->GetProperty()->SetColor(impl->m_setting.color);
	} else {
		auto value = iRIC::toStr(impl->m_setting.value);
		data->GetCellData()->SetActiveScalars(value.c_str());
		auto cs = impl->m_colorMapSettings.at(value);
		auto mapper = cs->buildCellDataMapper(data, false);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

		auto is = cs->legendSetting()->imgSetting();
		is->setActor(impl->m_legendActor);
		is->controller()->setItem(this);
		is->apply(dataModel()->graphicsView());
	}
	auto v = dataModel()->graphicsView();
	impl->m_actor->GetProperty()->SetLineWidth(impl->m_setting.lineWidth * v->devicePixelRatioF());
	impl->m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);
	m_actorCollection->AddItem(impl->m_actor);

	updateCheckState();
	updateVisibilityWithoutRendering();
}

void Post2dWindowPolyDataGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowPolyDataGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Polygon Data Display Setting"));
	dialog->resize(900, 650);

	return dialog;
}

void Post2dWindowPolyDataGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowPolyDataGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowPolyDataGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->initialize();
}

void Post2dWindowPolyDataGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->clear();
}

void Post2dWindowPolyDataGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

void Post2dWindowPolyDataGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
	}

	attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowPolyDataGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
	}
}

void Post2dWindowPolyDataGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s != nullptr) {
		s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
	}

	attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowPolyDataGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_showAttributeBrowserAction);
}

bool Post2dWindowPolyDataGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_polyDataToolBarWidget->setParent(toolBar);
	impl->m_polyDataToolBarWidget->setEnabled(true);
	impl->m_polyDataToolBarWidget->show();

	toolBar->addWidget(impl->m_polyDataToolBarWidget);

	return true;
}

void Post2dWindowPolyDataGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("PolyData value change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowPolyDataGroupDataItem::showAttributeBrowser()
{
	attributeBrowserController()->initialize();
	auto w = dynamic_cast<Post2dWindow*>(mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowPolyDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		if (child.nodeName() == "ColorMapSetting") {
			auto elem = child.toElement();
			auto name = iRIC::toStr(elem.attribute("name"));
			auto it = impl->m_colorMapSettings.find(name);
			if (it == impl->m_colorMapSettings.end()) {continue;}

			it->second->load(child);
		}
	}

	updateCheckState();
	updateActorSetting();
}

void Post2dWindowPolyDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);

	for (const auto& pair : impl->m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

ColorMapSettingContainerI* Post2dWindowPolyDataGroupDataItem::activeSetting() const
{
	if (impl->m_setting.mapping == PolyDataSetting::Mapping::Arbitrary) {return nullptr;}

	return impl->m_colorMapSettings.at(iRIC::toStr(impl->m_setting.value));
}

Post2dWindowGridTypeDataItem* Post2dWindowPolyDataGroupDataItem::gridTypeDataItem() const
{
	return zoneDataItem()->gridTypeDataItem();
}

Post2dWindowZoneDataItem* Post2dWindowPolyDataGroupDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

Post2dWindowPolyDataTopDataItem* Post2dWindowPolyDataGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowPolyDataTopDataItem*> (parent());
}

v4PolyData2d* Post2dWindowPolyDataGroupDataItem::polyData() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return dynamic_cast<v4PolyData2d*> (cont->polyData(name())->grid());
}

void Post2dWindowPolyDataGroupDataItem::updateCheckState()
{
	if (impl->m_setting.mapping == PolyDataSetting::Mapping::Arbitrary) {
		NamedGraphicsWindowDataItemTool::checkItemWithName("", m_childItems, true);
	} else if (impl->m_setting.mapping == PolyDataSetting::Mapping::Value) {
		NamedGraphicsWindowDataItemTool::checkItemWithName(impl->m_setting.value, m_childItems, true);
	}
}
