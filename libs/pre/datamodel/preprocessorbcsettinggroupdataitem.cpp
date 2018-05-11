#include "preprocessorbcdataitem.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorbcsettingdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <QMainWindow>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <iriclib.h>

PreProcessorBCSettingGroupDataItem::PreProcessorBCSettingGroupDataItem(PreProcessorDataItem* parent) :
	PreProcessorDataItem {tr("Boundary Condition Setting"), QIcon(":/libs/guibase/images/iconFolder.png"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setSubPath("bcsetting");

	m_dummyEditAction = new QAction("&Edit Condition...", this);
	m_dummyEditAction->setDisabled(true);
	m_dummyDeleteAction = new QAction("&Delete...", this);
	m_dummyDeleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	m_dummyDeleteAction->setDisabled(true);
	setupAddActions();
}

PreProcessorBCSettingGroupDataItem::~PreProcessorBCSettingGroupDataItem()
{}

void PreProcessorBCSettingGroupDataItem::loadFromCgnsFile(const int /*fn*/)
{}

void PreProcessorBCSettingGroupDataItem::saveToCgnsFile(const int /*fn*/)
{}

void PreProcessorBCSettingGroupDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PreProcessorBCSettingGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (int i = 0; i < m_childItems.size(); ++i) {
		writer.writeStartElement("BoundaryConditionSetting");
		m_childItems[i]->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorBCSettingGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	for (int i = 0; i < m_addActions.count(); ++i) {
		menu->addAction(m_addActions[i]);
	}
}

void PreProcessorBCSettingGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(5);
}

void PreProcessorBCSettingGroupDataItem::updateItems()
{
	PreProcessorGridAndGridCreatingConditionDataItem* gagItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(gagItem->gridDataItem());
	if (gItem == nullptr) {return;}
	PreProcessorBCGroupDataItem* bcgitem = gItem->bcGroupDataItem();
	if (bcgitem == nullptr) {return;}

	m_itemMap.clear();
	int rows = m_standardItem->rowCount();
	for (int i = rows - 1; i >= 0; --i) {
		m_standardItem->takeRow(i);
	}
	// setup current children set as tmpItemSet.
	QMap<PreProcessorBCDataItem*, PreProcessorBCSettingDataItem*> tmpItemMap;

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorBCSettingDataItem* item = dynamic_cast<PreProcessorBCSettingDataItem*>(*it);
		tmpItemMap.insert(item->bcDataItem(), item);
	}
	m_childItems.clear();
	const auto& children = bcgitem->childItems();
	for (auto it = children.begin(); it != children.end(); ++it) {
		PreProcessorBCDataItem* bcItem = dynamic_cast<PreProcessorBCDataItem*>(*it);
		PreProcessorBCSettingDataItem* bcsItem = tmpItemMap.value(bcItem, 0);
		if (bcsItem == nullptr) {
			bcsItem = new PreProcessorBCSettingDataItem(bcItem, this);
		} else {
			if (! bcsItem->bcDataItem()->hideSetting()) {
				m_standardItem->appendRow(bcsItem->standardItem());
			}
			tmpItemMap.remove(bcItem);
		}
		m_childItems.push_back(bcsItem);
		m_itemMap.insert(bcItem, bcsItem);
	}
	// needless items removed.
	for (auto mit = tmpItemMap.begin(); mit != tmpItemMap.end(); ++mit) {
		delete mit.value();
	}
	updateItemMap();
	updateZDepthRange();
}

void PreProcessorBCSettingGroupDataItem::loadItems()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorBCSettingDataItem* item = dynamic_cast<PreProcessorBCSettingDataItem*>(*it);
		item->loadData();
	}
}

void PreProcessorBCSettingGroupDataItem::setupAddActions()
{
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	SolverDefinitionGridType* gtype = gtItem->gridType();
	for (int i = 0; i < gtype->boundaryConditions().count(); ++i) {
		SolverDefinitionBoundaryCondition* bc = gtype->boundaryConditions().at(i);
		QString str(tr("Add %1"));
		QAction* addAction = new QAction(str.arg(bc->caption()), this);
		connect(addAction, SIGNAL(triggered()), this, SLOT(addCondition()));
		m_addActions.append(addAction);
	}
}

void PreProcessorBCSettingGroupDataItem::addCondition()
{
	QAction* a = dynamic_cast<QAction*>(sender());
	int index = 0;
	for (index = 0; index < m_addActions.count(); ++index) {
		if (m_addActions[index] == a) {break;}
	}
	if (index >= m_addActions.count()) {return;}

	PreProcessorGridAndGridCreatingConditionDataItem* item = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(item->gridDataItem());
	PreProcessorBCDataItem* bcItem = gItem->bcGroupDataItem()->addCondition(index);
	PreProcessorBCSettingDataItem* bcsItem = m_itemMap.value(bcItem);

	dataModel()->objectBrowserView()->expand(m_standardItem->index());
	dataModel()->objectBrowserView()->select(bcsItem->standardItem()->index());
	dataModel()->handleObjectBrowserSelection(bcsItem->standardItem()->index());
}

void PreProcessorBCSettingGroupDataItem::executeMapping(bool noDraw)
{
	PreProcessorGridAndGridCreatingConditionDataItem* gccdItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	PreProcessorGridDataItemInterface* gitem = gccdItem->gridDataItem();
	Grid* grid = gitem->grid();
	if (grid == nullptr && ! noDraw) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("Mapping can not be executed when there is no grid."));
		return;
	}

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorBCSettingDataItem* item = dynamic_cast<PreProcessorBCSettingDataItem*>(*it);
		item->executeMapping(true, 0);
	}
	if (! noDraw) {
		renderGraphicsView();
	}
}
