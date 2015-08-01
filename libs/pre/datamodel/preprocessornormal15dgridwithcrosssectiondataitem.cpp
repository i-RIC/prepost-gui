#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "preprocessornormal15dgridwithcrosssectionshapedataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QAction>
#include <QMenu>

PreProcessorNormal15DGridWithCrossSectionDataItem::PreProcessorNormal15DGridWithCrossSectionDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridDataItem {parent}
{
	m_shapeDataItem = new PreProcessorNormal15DGridWithCrossSectionShapeDataItem(this);
	m_childItems.append(m_shapeDataItem);
	m_standardItem->takeRow(m_shapeDataItem->standardItem()->row());
	m_nodeGroupDataItem = new PreProcessorGridAttributeNodeGroupDataItem(this);
	m_childItems.append(m_nodeGroupDataItem);
	m_standardItem->takeRow(m_nodeGroupDataItem->standardItem()->row());
	m_cellGroupDataItem = new PreProcessorGridAttributeCellGroupDataItem(this);
	m_childItems.append(m_cellGroupDataItem);
	m_standardItem->takeRow(m_cellGroupDataItem->standardItem()->row());
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent->parent());
	if (gtItem->gridType()->boundaryConditions().count() > 0) {
		m_bcGroupDataItem = new PreProcessorBCGroupDataItem(this);
		m_childItems.append(m_bcGroupDataItem);
		m_standardItem->takeRow(m_bcGroupDataItem->standardItem()->row());
	} else {
		m_bcGroupDataItem = nullptr;
	}
	setupMenu();
}

PreProcessorNormal15DGridWithCrossSectionDataItem::~PreProcessorNormal15DGridWithCrossSectionDataItem()
{}

void PreProcessorNormal15DGridWithCrossSectionDataItem::setupMenu()
{
	m_editMenu = m_menu->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_shapeDataItem->editAction());
	m_editMenu->addAction(m_nodeEditAction);
	m_editMenu->addAction(m_cellEditAction);

	if (m_bcGroupDataItem != nullptr) {
		m_menu->addMenu(m_bcGroupDataItem->bcMenu());
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
