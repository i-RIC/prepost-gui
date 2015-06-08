#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorunstructured2dgriddataitem.h"
#include "preprocessorunstructured2dgridshapedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QAction>
#include <QMenu>

PreProcessorUnstructured2dGridDataItem::PreProcessorUnstructured2dGridDataItem(PreProcessorDataItem* parent)
	: PreProcessorGridDataItem(parent)
{
	m_shapeDataItem = new PreProcessorUnstructured2dGridShapeDataItem(this);
	m_childItems.append(m_shapeDataItem);
	m_nodeGroupDataItem = new PreProcessorGridAttributeNodeGroupDataItem(this);
	m_childItems.append(m_nodeGroupDataItem);
	m_cellGroupDataItem = new PreProcessorGridAttributeCellGroupDataItem(this);
	m_childItems.append(m_cellGroupDataItem);

	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent->parent());
	if (gtItem->gridType()->boundaryConditions().count() > 0) {
		m_bcGroupDataItem = new PreProcessorBCGroupDataItem(this);
		m_childItems.append(m_bcGroupDataItem);
	} else {
		m_bcGroupDataItem = 0;
	}
	setupMenu();
	updateObjectBrowserTree();
}

PreProcessorUnstructured2dGridDataItem::~PreProcessorUnstructured2dGridDataItem()
{}

void PreProcessorUnstructured2dGridDataItem::setupMenu()
{
	m_selectMenu = m_menu->addMenu(tr("&Select vertices"));
	m_selectMenu->addAction(m_polygonSelectAction);

	m_editMenu = m_menu->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_shapeDataItem->editAction());
	m_editMenu->addAction(m_nodeEditAction);
	m_editMenu->addAction(m_cellEditAction);

	if (m_bcGroupDataItem != 0) {
		m_editMenu->addMenu(m_bcGroupDataItem->bcMenu());
	}
	m_menu->addAction(m_deleteAction);
	m_menu->addSeparator();

	m_displayMenu = m_menu->addMenu(tr("Dis&play Setting..."));
	m_displayMenu->addAction(m_displaySettingAction);
	m_displayMenu->addAction(m_nodeDisplaySettingAction);
	m_displayMenu->addAction(m_cellDisplaySettingAction);
	m_displayMenu->addSeparator();
	m_displayMenu->addAction(m_setupScalarBarAction);
}

void PreProcessorUnstructured2dGridDataItem::updateActionStatus()
{
	m_selectMenu->setEnabled(m_grid != 0);
	m_editMenu->setEnabled(m_grid != 0);
	m_displayMenu->setEnabled(m_grid != 0);

	PreProcessorGridDataItem::updateActionStatus();
}
