#include "preprocessorbcdataitem.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorbcsettingdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemmultiselectingdialog.h>
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
	PreProcessorDataItem {tr("Boundary Condition Setting"), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_deleteSelectedAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), PreProcessorBCGroupDataItem::tr("Delete &Selected..."), this)},
	m_deleteAllAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), PreProcessorBCGroupDataItem::tr("Delete &All..."), this)},
	m_dummyEditAction {new QAction("&Edit Condition...", this)},
	m_dummyDeleteAction {new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), "&Delete...", this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setSubPath("bcsetting");

	m_dummyEditAction->setDisabled(true);
	m_dummyDeleteAction->setDisabled(true);
	setupAddActions();

	connect(m_deleteSelectedAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
}

PreProcessorBCSettingGroupDataItem::~PreProcessorBCSettingGroupDataItem()
{}

void PreProcessorBCSettingGroupDataItem::loadFromCgnsFile(const int /*fn*/)
{}

void PreProcessorBCSettingGroupDataItem::saveToCgnsFile(const int /*fn*/)
{}

void PreProcessorBCSettingGroupDataItem::deleteSelected()
{
	if (m_childItems.size() == 0) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("There is no boundary condition setting."), QMessageBox::Ok);
		return;
	}

	std::vector<PreProcessorBCSettingDataItem*> items;

	std::vector<QString> names;
	for (auto item : m_childItems) {
		auto bcSettingItem = dynamic_cast<PreProcessorBCSettingDataItem*> (item);
		if (bcSettingItem->standardItem() == nullptr) {continue;}

		items.push_back(bcSettingItem);
		names.push_back(bcSettingItem->standardItem()->text());
	}

	ItemMultiSelectingDialog dialog(mainWindow());
	dialog.setWindowTitle(tr("Delete selected boundary condition settings"));
	dialog.setItems(names);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto settings = dialog.selectSettings();
	for (int i = 0; i < settings.size(); ++i) {
		if (settings.at(i)) {
			// delete the item
			delete items.at(i)->bcDataItem();
		}
	}
}

void PreProcessorBCSettingGroupDataItem::deleteAll()
{
	int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all boundary condition settings?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	auto items = m_childItems;
	for (auto item : items) {
		auto bcSettingItem = dynamic_cast<PreProcessorBCSettingDataItem*> (item);
		if (bcSettingItem->standardItem() == nullptr) {continue;}
		delete bcSettingItem->bcDataItem();
	}
}

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
	menu->addSeparator();
	menu->addAction(m_deleteSelectedAction);
	menu->addAction(m_deleteAllAction);
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

const QList<QAction*>& PreProcessorBCSettingGroupDataItem::addActions() const
{
	return m_addActions;
}

QAction* PreProcessorBCSettingGroupDataItem::dummyEditAction() const
{
	return m_dummyEditAction;
}

QAction* PreProcessorBCSettingGroupDataItem::dummyDeleteAction() const
{
	return m_dummyDeleteAction;
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
