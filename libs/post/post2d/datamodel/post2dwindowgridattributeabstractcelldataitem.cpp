#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridattributeabstractcelldataitem.h"
#include "post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridattributeabstractcelldataitem_propertydialog.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/qwidgetcontainer.h>
#include <misc/valuechangert.h>

Post2dWindowGridAttributeAbstractCellDataItem::Post2dWindowGridAttributeAbstractCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(cond->name(), cond->caption(), parent),
	m_condition {cond}
{
	m_colorMapToolBarWidgetController = gridTypeDataItem()->createToolBarWidgetController(cond->name(), mainWindow());
}

Post2dWindowGridAttributeAbstractCellDataItem::~Post2dWindowGridAttributeAbstractCellDataItem()
{}

QDialog* Post2dWindowGridAttributeAbstractCellDataItem::propertyDialog(QWidget* p)
{
	auto compAtt = dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	if (compAtt != nullptr && compAtt->isGrouped() == false) {
		return nullptr;
	}

	auto setting = colorMapSettingContainer();
	if (setting == nullptr) {return nullptr;}

	auto gItem = groupDataItem();
	auto dialog = new PropertyDialog(gItem, p);
	dialog->setWindowTitle(tr("Grid %1 Attribute Display Setting (%2)").arg(positionCaption()).arg(condition()->caption()));
	auto widget = m_condition->createColorMapSettingEditWidget(dialog);
	widget->setSetting(setting);
	dialog->setWidget(widget);

	dialog->setLineWidth(gItem->lineWidth());
	dialog->setOpacity(gItem->opacity());
	dialog->resize(900, 700);

	return dialog;
}

void Post2dWindowGridAttributeAbstractCellDataItem::updateVisibility(bool /*visible*/)
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);
	gridTypeDataItem()->updateColorBarVisibility(condition()->name());
}

void Post2dWindowGridAttributeAbstractCellDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
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

void Post2dWindowGridAttributeAbstractCellDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
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

void Post2dWindowGridAttributeAbstractCellDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto setting = colorMapSettingContainer();
	if (setting != nullptr && setting->legendSetting()->getVisible()) {
		auto imgCtrl = setting->legendSetting()->imgSetting()->controller();
		imgCtrl->handleMouseReleaseEvent(this, event, v);
	}
	static QMenu* menu = nullptr;
	if (event->button() == Qt::LeftButton) {
		groupDataItem()->fixAttributeBrowser(event->pos(), v);
	} else if (event->button() == Qt::RightButton) {
		delete menu;
		menu = new QMenu(mainWindow());
		menu->addAction(groupDataItem()->showAttributeBrowserAction());
		menu->move(event->globalPos());
		menu->show();
	}
}

void Post2dWindowGridAttributeAbstractCellDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(groupDataItem()->showAttributeBrowserAction());
}

void Post2dWindowGridAttributeAbstractCellDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void Post2dWindowGridAttributeAbstractCellDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->initAttributeBrowser();

	GraphicsWindowDataItem::updateVisibility();
}

void Post2dWindowGridAttributeAbstractCellDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->clearAttributeBrowser();
}

SolverDefinitionGridAttribute* Post2dWindowGridAttributeAbstractCellDataItem::condition() const
{
	return m_condition;
}

Post2dWindowGridTypeDataItem* Post2dWindowGridAttributeAbstractCellDataItem::gridTypeDataItem() const
{
	return gridDataItem()->zoneDataItem()->gridTypeDataItem();
}

Post2dWindowGeoDataGroupDataItem* Post2dWindowGridAttributeAbstractCellDataItem::geoDataGroupDataItem() const
{
	return gridTypeDataItem()->geoDataItem()->groupDataItem(m_condition->name());
}

Post2dWindowGridAttributeAbstractCellGroupDataItem* Post2dWindowGridAttributeAbstractCellDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowGridAttributeAbstractCellGroupDataItem*> (parent());
}

Post2dWindowInputGridDataItem* Post2dWindowGridAttributeAbstractCellDataItem::gridDataItem() const
{
	return groupDataItem()->gridDataItem();
}

bool Post2dWindowGridAttributeAbstractCellDataItem::addToolBarButtons(QToolBar* toolBar)
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
	auto att = grid->attribute(condition()->name());
	const auto& selectWidgets = att->dimensions()->selectWidgets();
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

ColorMapSettingContainerI* Post2dWindowGridAttributeAbstractCellDataItem::colorMapSettingContainer() const
{
	return gridTypeDataItem()->colorMapSetting(m_condition->name())->customSetting;
}

ColorMapSettingToolBarWidgetController* Post2dWindowGridAttributeAbstractCellDataItem::colorMapSettingToolBarWidgetController() const
{
	return m_colorMapToolBarWidgetController;
}
