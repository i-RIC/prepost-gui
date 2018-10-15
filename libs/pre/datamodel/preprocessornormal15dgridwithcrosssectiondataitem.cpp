#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "preprocessornormal15dgridwithcrosssectionshapedataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QAction>
#include <QStandardItem>
#include <QMenu>

PreProcessorNormal15DGridWithCrossSectionDataItem::PreProcessorNormal15DGridWithCrossSectionDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridDataItem {parent}
{
	m_shapeDataItem = new PreProcessorNormal15DGridWithCrossSectionShapeDataItem(this);
	m_childItems.push_back(m_shapeDataItem);
	m_standardItem->takeRow(m_shapeDataItem->standardItem()->row());
	m_nodeGroupDataItem = new PreProcessorGridAttributeNodeGroupDataItem(this);
	m_childItems.push_back(m_nodeGroupDataItem);
	m_standardItem->takeRow(m_nodeGroupDataItem->standardItem()->row());
	m_cellGroupDataItem = new PreProcessorGridAttributeCellGroupDataItem(this);
	m_childItems.push_back(m_cellGroupDataItem);
	m_standardItem->takeRow(m_cellGroupDataItem->standardItem()->row());
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent->parent());
	if (gtItem->gridType()->boundaryConditions().count() > 0) {
		m_bcGroupDataItem = new PreProcessorBCGroupDataItem(this);
		m_childItems.push_back(m_bcGroupDataItem);
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
	m_editMenu = menu()->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_shapeDataItem->editAction());
	m_editMenu->addAction(nodeEditAction());
	m_editMenu->addAction(cellEditAction());

	if (m_bcGroupDataItem != nullptr) {
		menu()->addMenu(m_bcGroupDataItem->bcMenu());
	}
	menu()->addAction(deleteAction());
	menu()->addSeparator();

	m_displayMenu = menu()->addMenu(tr("Dis&play Setting..."));
	m_displayMenu->addAction(displaySettingAction());
	m_displayMenu->addAction(nodeDisplaySettingAction());
	m_displayMenu->addAction(cellDisplaySettingAction());
	m_displayMenu->addSeparator();
	m_displayMenu->addAction(setupScalarBarAction());
}
