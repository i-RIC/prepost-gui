#include "../post2dwindowattributebrowsercontroller.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowabstractcellscalargroupdataitem.h"
#include "post2dwindowabstractcellscalargrouptopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_impl.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/objectbrowserview.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainerutil.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowAbstractCellScalarGroupDataItem::Post2dWindowAbstractCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, Reorderable, Deletable);

	impl->m_target = target;

	auto gType = topDataItem()->zoneDataItem()->v4DataContainer()->gridType();
	QString caption = gType->outputCaption(target);
	SolverDefinitionGridOutput* output = gType->output(target);
	standardItem()->setText(caption);
	auto cs = output->createColorMapSettingContainer();
	cs->legendSetting()->setTitle(caption);
	impl->m_setting.colorMapSetting = cs;

	impl->m_colorMapToolBarWidget->hide();
	impl->m_colorMapToolBarWidget->setSetting(impl->m_setting.colorMapSetting);
	connect(impl->m_colorMapToolBarWidget, &ColorMapSettingToolBarWidget::updated, [=](){
		auto com = new ColorMapSettingModifyCommand(impl->m_colorMapToolBarWidget->modifiedSetting(), impl->m_setting.colorMapSetting);
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

Post2dWindowAbstractCellScalarGroupDataItem::~Post2dWindowAbstractCellScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);
	r->RemoveActor2D(impl->m_legendActor);
}

const std::string& Post2dWindowAbstractCellScalarGroupDataItem::target() const
{
	return impl->m_target;
}

void Post2dWindowAbstractCellScalarGroupDataItem::updateActorSetting()
{
	impl->m_actor->VisibilityOff();

	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOffWithEmptyPolyData();
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
		return;
	}

	auto range = valueRange(impl->m_target);
	impl->m_setting.colorMapSetting->setAutoValueRange(range);

	vtkPointSet* filtered = nullptr;
	if (impl->m_setting.regionSetting.mode == Region2dSettingContainer::Mode::Full) {
		filtered = filteredData();
		if (filtered != nullptr) {
			filtered->Register(nullptr);
		}
	} else {
		filtered = buildRegionFilteredData();
	}
	if (filtered == nullptr) {return;}

	filtered->GetCellData()->SetActiveScalars(impl->m_target.c_str());
	auto mapper = impl->m_setting.colorMapSetting->buildCellDataMapper(filtered, false);
	impl->m_actor->SetMapper(mapper);

	filtered->Delete();
	mapper->Delete();

	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->apply(dataModel()->graphicsView());
	impl->m_actor->GetProperty()->SetOpacity(impl->m_setting.opacity);
	impl->m_actor->GetProperty()->SetLineWidth(impl->m_setting.lineWidth);

	m_actor2DCollection->RemoveAllItems();
	if (impl->m_setting.colorMapSetting->legendSetting()->getVisible()) {
		m_actor2DCollection->AddItem(impl->m_legendActor);
	}

	updateVisibilityWithoutRendering();
}

void Post2dWindowAbstractCellScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	updateActorSetting();
}

void Post2dWindowAbstractCellScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

void Post2dWindowAbstractCellScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(impl->m_actor);
	r->AddActor2D(impl->m_legendActor);

	m_actorCollection->AddItem(impl->m_actor);

	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->setActor(impl->m_legendActor);
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->setItem(this);
}

void Post2dWindowAbstractCellScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowAbstractCellScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowAbstractCellScalarGroupDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowAbstractCellScalarGroupDataItem::innerUpdateZScale(double scale)
{
	impl->m_actor->SetScale(1, scale, 1);
}

Post2dWindowAbstractCellScalarGroupTopDataItem* Post2dWindowAbstractCellScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowAbstractCellScalarGroupTopDataItem*> (parent());
}

void Post2dWindowAbstractCellScalarGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowAbstractCellScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	dialog->resize(900, 700);

	return dialog;
}

bool Post2dWindowAbstractCellScalarGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (impl->m_setting.opacity == 100) {return false;}

	return true;
}

void Post2dWindowAbstractCellScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	topDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowAbstractCellScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	topDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowAbstractCellScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
	topDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowAbstractCellScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void Post2dWindowAbstractCellScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
	if (event->button() == Qt::LeftButton) {
		topDataItem()->attributeBrowserController()->fix(event->pos(), v);
	}
}

void Post2dWindowAbstractCellScalarGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	impl->m_setting.colorMapSetting->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void Post2dWindowAbstractCellScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(topDataItem()->showAttributeBrowserAction());
}

bool Post2dWindowAbstractCellScalarGroupDataItem::addToolBarButtons(QToolBar* toolBar)
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

void Post2dWindowAbstractCellScalarGroupDataItem::updateMoveUpDownActions(ObjectBrowserView* view)
{
	auto parentItem = dynamic_cast<GraphicsWindowDataItem*>(parent())->standardItem();
	bool isFirst = (m_standardItem->index().row() == 0);
	bool isLast = (m_standardItem->index().row() == parentItem->rowCount() - 1);
	view->moveUpAction()->setDisabled(isFirst);
	view->moveDownAction()->setDisabled(isLast);
}
