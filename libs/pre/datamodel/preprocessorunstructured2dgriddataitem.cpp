#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorunstructured2dgriddataitem.h"
#include "preprocessorunstructured2dgridshapedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QAction>
#include <QMenu>

PreProcessorUnstructured2dGridDataItem::PreProcessorUnstructured2dGridDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridDataItem {parent}
{
	m_shapeDataItem = new PreProcessorUnstructured2dGridShapeDataItem(this);
	m_childItems.push_back(m_shapeDataItem);
	m_nodeGroupDataItem = new PreProcessorGridAttributeNodeGroupDataItem(this);
	m_childItems.push_back(m_nodeGroupDataItem);
	m_cellGroupDataItem = new PreProcessorGridAttributeCellGroupDataItem(this);
	m_childItems.push_back(m_cellGroupDataItem);

	setupMenu();
	updateObjectBrowserTree();
}

PreProcessorUnstructured2dGridDataItem::~PreProcessorUnstructured2dGridDataItem()
{}

void PreProcessorUnstructured2dGridDataItem::setupMenu()
{
	m_selectMenu = menu()->addMenu(tr("&Select vertices"));
	m_selectMenu->addAction(polygonSelectAction());

	m_editMenu = menu()->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_shapeDataItem->editAction());
	m_editMenu->addAction(nodeEditAction());
	m_editMenu->addAction(cellEditAction());

	if (m_bcGroupDataItem != nullptr) {
		m_editMenu->addMenu(m_bcGroupDataItem->bcMenu());
	}
	menu()->addAction(deleteAction());
	menu()->addSeparator();

	m_displayMenu = menu()->addMenu(tr("Dis&play Setting..."));
	m_displayMenu->addAction(displaySettingAction());
	m_displayMenu->addAction(nodeDisplaySettingAction());
	m_displayMenu->addAction(cellDisplaySettingAction());
}

void PreProcessorUnstructured2dGridDataItem::updateActionStatus()
{
	m_selectMenu->setEnabled(grid() != nullptr);
	m_editMenu->setEnabled(grid() != nullptr);
	m_displayMenu->setEnabled(grid() != nullptr);

	PreProcessorGridDataItem::updateActionStatus();
}
