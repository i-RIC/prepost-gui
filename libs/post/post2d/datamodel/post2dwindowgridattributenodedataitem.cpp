#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridattributenodedataitem.h"
#include "post2dwindowgridattributenodegroupdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridattributenodedataitem_propertydialog.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <misc/qwidgetcontainer.h>
#include <misc/valuechangert.h>

Post2dWindowGridAttributeNodeDataItem::Post2dWindowGridAttributeNodeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(cond->name(), cond->caption(), parent)
{
	m_condition = cond;

	m_colorMapToolBarWidgetController = gridTypeDataItem()->createToolBarWidgetController(cond->name(), mainWindow());
}

Post2dWindowGridAttributeNodeDataItem::~Post2dWindowGridAttributeNodeDataItem()
{}

QDialog* Post2dWindowGridAttributeNodeDataItem::propertyDialog(QWidget* p)
{
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		return nullptr;
	}

	auto setting = colorMapSettingContainer();
	if (setting == nullptr) {return nullptr;}

	auto gItem = groupDataItem();
	auto dialog = new PropertyDialog(gItem, p);
	dialog->setWindowTitle(tr("Grid Node Attribute Display Setting (%1)").arg(condition()->caption()));
	auto widget = m_condition->createColorMapSettingEditWidget(dialog);
	widget->setSetting(setting);
	dialog->setWidget(widget);

	dialog->setOpacity(gItem->opacity());
	dialog->resize(900, 700);

	return dialog;
}

void Post2dWindowGridAttributeNodeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseMoveEvent(this, event, v);
		if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
			return;
		}
	}

	groupDataItem()->updateAttributeBrowser(event->pos(), v);
}

void Post2dWindowGridAttributeNodeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMousePressEvent(this, event, v);
		if (imgCtrl->mouseEventMode() != ImageSettingContainer::Controller::MouseEventMode::Normal) {
			return;
		}
	}
}

void Post2dWindowGridAttributeNodeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseReleaseEvent(this, event, v);
	}
	static QMenu* menu = nullptr;
	if (event->button() == Qt::LeftButton) {
		groupDataItem()->fixAttributeBrowser(event->pos(), v);
	}
	if (event->button() == Qt::RightButton) {
		delete menu;
		menu = new QMenu(mainWindow());
		menu->addAction(groupDataItem()->showAttributeBrowserAction());
		menu->move(event->globalPos());
		menu->show();
	}
}

void Post2dWindowGridAttributeNodeDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(groupDataItem()->showAttributeBrowserAction());
}

void Post2dWindowGridAttributeNodeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->initAttributeBrowser();

	GraphicsWindowDataItem::updateVisibility();
}

void Post2dWindowGridAttributeNodeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->clearAttributeBrowser();
}

SolverDefinitionGridAttribute* Post2dWindowGridAttributeNodeDataItem::condition() const
{
	return m_condition;
}

ColorMapSettingContainerI* Post2dWindowGridAttributeNodeDataItem::colorMapSettingContainer() const
{
	return gridTypeDataItem()->colorMapSetting(m_condition->name())->customSetting;
}

ColorMapSettingToolBarWidgetController* Post2dWindowGridAttributeNodeDataItem::colorMapSettingToolBarWidgetController() const
{
	return m_colorMapToolBarWidgetController;
}

void Post2dWindowGridAttributeNodeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post2dWindowGridAttributeNodeDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	groupDataItem()->updateActorSetting();
}

void Post2dWindowGridAttributeNodeDataItem::updateVisibility(bool /*visible*/)
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);
	gridTypeDataItem()->updateColorBarVisibility(condition()->name());
}

Post2dWindowGridTypeDataItem* Post2dWindowGridAttributeNodeDataItem::gridTypeDataItem() const
{
	return gridDataItem()->zoneDataItem()->gridTypeDataItem();
}

Post2dWindowGeoDataGroupDataItem* Post2dWindowGridAttributeNodeDataItem::geoDataGroupDataItem() const
{
	return gridTypeDataItem()->geoDataItem()->groupDataItem(m_condition->name());
}

Post2dWindowGridAttributeNodeGroupDataItem* Post2dWindowGridAttributeNodeDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowGridAttributeNodeGroupDataItem*> (parent());
}

Post2dWindowInputGridDataItem* Post2dWindowGridAttributeNodeDataItem::gridDataItem() const
{
	return groupDataItem()->gridDataItem();
}

bool Post2dWindowGridAttributeNodeDataItem::addToolBarButtons(QToolBar* toolBar)
{
	auto opacityW = groupDataItem()->opacityWidget();
	opacityW->setParent(toolBar);
	opacityW->show();
	toolBar->addWidget(opacityW);

	toolBar->addSeparator();

	auto cmwContainer = groupDataItem()->colorMapWidgetContainer();
	cmwContainer->setParent(toolBar);
	cmwContainer->show();
	toolBar->addWidget(cmwContainer);
	cmwContainer->setWidget(m_colorMapToolBarWidgetController->widget());

	v4InputGrid* grid = groupDataItem()->gridDataItem()->inputGrid();
	auto cont = grid->attribute(condition()->name());
	const auto& selectWidgets = cont->dimensions()->selectWidgets();
	if (selectWidgets.size() > 0) {
		toolBar->addSeparator();

		for (int i = 0; i < static_cast<int> (selectWidgets.size()); ++i) {
			auto w = selectWidgets.at(i);
			QAction* action = toolBar->addWidget(w);
			action->setVisible(true);
		}
	}

	return true;
}
