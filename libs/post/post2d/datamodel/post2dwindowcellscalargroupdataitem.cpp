#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowcellscalargroupdataitem_impl.h"
#include "private/post2dwindowcellscalargroupdataitem_settingeditwidget.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainerutil.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowCellScalarGroupDataItem::Post2dWindowCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, Reorderable, Deletable);

	impl->m_target = target;

	auto gType = topDataItem()->zoneDataItem()->dataContainer()->gridType();
	standardItem()->setText(gType->output(target)->caption());

	impl->m_colorMapToolBarWidget->hide();
	impl->m_colorMapToolBarWidget->setSetting(&impl->m_setting.colorMapSetting);
	connect(impl->m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(impl->m_colorMapToolBarWidget->modifiedSetting(), &impl->m_setting.colorMapSetting);
		pushUpdateActorSettingCommand(com, this);
	});

	impl->m_opacityToolBarWidget->hide();
	impl->m_opacityToolBarWidget->setContainer(&impl->m_setting.opacity);
	connect(impl->m_opacityToolBarWidget, &OpacityContainerWidget::updated, [=](){
		auto com = impl->m_opacityToolBarWidget->createModifyCommand();
		pushUpdateActorSettingCommand(com, this, false);
	});

	setupActors();
}

Post2dWindowCellScalarGroupDataItem::~Post2dWindowCellScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);
	r->RemoveActor2D(impl->m_legendActor);

	delete impl;
}

const std::string& Post2dWindowCellScalarGroupDataItem::target() const
{
	return impl->m_target;
}

void Post2dWindowCellScalarGroupDataItem::updateActorSetting()
{
	impl->m_actor->VisibilityOff();

	auto z = topDataItem()->zoneDataItem();
	auto cont = z->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOffWithEmptyPolyData();
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
		return;
	}

	auto range = z->gridTypeDataItem()->cellValueRange(impl->m_target);
	impl->m_setting.colorMapSetting.setAutoValueRange(range);

	vtkPointSet* filtered = nullptr;
	if (impl->m_setting.regionSetting.mode == Region2dSettingContainer::Mode::Full) {
		filtered = z->filteredData();
		if (filtered != nullptr) {
			filtered->Register(nullptr);
		}
	} else {
		filtered = impl->m_setting.regionSetting.buildCellFilteredData(cont->data()->data());
	}
	if (filtered == nullptr) {return;}

	filtered->GetCellData()->SetActiveScalars(impl->m_target.c_str());
	auto mapper = impl->m_setting.colorMapSetting.buildCellDataMapper(filtered, false);
	impl->m_actor->SetMapper(mapper);

	filtered->Delete();
	mapper->Delete();

	impl->m_setting.colorMapSetting.legend.imageSetting.apply(dataModel()->graphicsView());
	impl->m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);

	m_actor2DCollection->RemoveAllItems();
	if (impl->m_setting.colorMapSetting.legend.visible) {
		m_actor2DCollection->AddItem(impl->m_legendActor);
	}

	updateVisibilityWithoutRendering();
}

void Post2dWindowCellScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	updateActorSetting();
}

void Post2dWindowCellScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void Post2dWindowCellScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_actor);
	r->AddActor2D(impl->m_legendActor);

	m_actorCollection->AddItem(impl->m_actor);

	impl->m_setting.colorMapSetting.legend.imageSetting.setActor(impl->m_legendActor);
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->setItem(this);
	impl->m_setting.colorMapSetting.legend.title = topDataItem()->zoneDataItem()->dataContainer()->gridType()->solutionCaption(target());

	updateActorSetting();
}

void Post2dWindowCellScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowCellScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowCellScalarGroupDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowCellScalarGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_actor->SetScale(1, scale, 1);
}

Post2dWindowCellScalarGroupTopDataItem* Post2dWindowCellScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowCellScalarGroupTopDataItem*> (parent());
}

void Post2dWindowCellScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowCellScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	dialog->resize(900, 700);

	return dialog;
}

bool Post2dWindowCellScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (impl->m_setting.opacity == 100) {return false;}

	return true;
}

void Post2dWindowCellScalarGroupDataItem::informSelection(VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
	topDataItem()->zoneDataItem()->initCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
	topDataItem()->zoneDataItem()->clearCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	topDataItem()->zoneDataItem()->updateCellResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowCellScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post2dWindowCellScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	if (event->button() == Qt::LeftButton) {
		topDataItem()->zoneDataItem()->fixCellResultAttributeBrowser(event->pos(), v);
	}
}

void Post2dWindowCellScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting.legend.imageSetting.controller()->handleResize(event, v);
}

void Post2dWindowCellScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = topDataItem()->zoneDataItem()->showAttributeBrowserActionForCellResult();
	menu->addAction(abAction);
}

bool Post2dWindowCellScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_colorMapToolBarWidget->setParent(toolBar);
	impl->m_colorMapToolBarWidget->show();

	toolBar->addWidget(impl->m_colorMapToolBarWidget);
	toolBar->addSeparator();

	impl->m_opacityToolBarWidget->setParent(toolBar);
	impl->m_opacityToolBarWidget->show();

	toolBar->addWidget(impl->m_opacityToolBarWidget);

	return true;
}

void Post2dWindowCellScalarGroupDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	auto parentItem = dynamic_cast<GraphicsWindowDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}
