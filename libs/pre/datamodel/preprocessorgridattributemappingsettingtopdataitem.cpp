#include "../preprocessorwindow.h"
#include "preprocessorbcdataitem.h"
#include "preprocessorbcsettingdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributecustommappingdialog.h"
#include "preprocessorgridattributemappingsettingdataitem.h"
#include "preprocessorgridattributemappingsettingtopdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"

#include <guibase/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QAction>
#include <QApplication>
#include <QDomElement>
#include <QIcon>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridAttributeMappingSettingTopDataItem::PreProcessorGridAttributeMappingSettingTopDataItem(PreProcessorDataItem* p) :
	PreProcessorDataItem {p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	// setup actions;
	m_executeMappingAction = new QAction(PreProcessorGridAttributeMappingSettingTopDataItem::tr("Execute mapping"), this);
	connect(m_executeMappingAction, SIGNAL(triggered()), this, SLOT(executeMapping()));

	m_customMappingAction = new QAction(PreProcessorGridAttributeMappingSettingTopDataItem::tr("&Execute..."), this);
	connect(m_customMappingAction, SIGNAL(triggered()), this, SLOT(customMapping()));

	// setup child items;
	PreProcessorGridTypeDataItem* gtItem =
		dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	SolverDefinitionGridType* gType = gtItem->gridType();
	QList<SolverDefinitionGridAttribute*> conditions = gType->gridRelatedConditions();
	for (auto it = conditions.begin(); it != conditions.end(); ++it) {
		PreProcessorGeoDataGroupDataItem* geodataGroup = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gtItem->geoDataTop()->groupDataItem((*it)->name()));
		PreProcessorGridAttributeMappingSettingDataItem* item = new PreProcessorGridAttributeMappingSettingDataItem(*it, geodataGroup, this);
		m_itemNameMap.insert((*it)->name(), item);
		m_childItems.append(item);
	}
	QList<SolverDefinitionGridComplexAttribute*> cconditions = gType->gridRelatedComplexConditions();
	for (auto cit = cconditions.begin(); cit != cconditions.end(); ++cit) {
		PreProcessorGeoDataGroupDataItem* geodataGroup = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gtItem->geoDataTop()->groupDataItem((*cit)->name()));
		PreProcessorGridAttributeMappingSettingDataItem* item = new PreProcessorGridAttributeMappingSettingDataItem(*cit, geodataGroup, this);
		m_itemNameMap.insert((*cit)->name(), item);
		m_childItems.append(item);
	}
}

void PreProcessorGridAttributeMappingSettingTopDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void PreProcessorGridAttributeMappingSettingTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}

void PreProcessorGridAttributeMappingSettingTopDataItem::informGeoDataChange()
{
	/*
		PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(parent()->parent()->parent());
		if (root->gridAttributeMappingMode() == PreProcessorRootDataItem::mmAuto){
			executeMapping();
		}
	 */
}

void PreProcessorGridAttributeMappingSettingTopDataItem::executeMapping()
{
	// Get the grid.
	PreProcessorGridAndGridCreatingConditionDataItem* conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	Grid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	if (! checkDimensions()) {return;}
	int count = 0;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*it);
		SolverDefinitionGridAttribute* cond = item->condition();
		GridAttributeContainer* container = grid->gridRelatedCondition(cond->name());
		if (!container->mapped()) {
			// mapping is done only when it is not mapped by the grid creating condition.
			count += item->mappingCount();
		}
	}

	WaitDialog dialog(preProcessorWindow());
	dialog.showProgressBar();
	dialog.disableCancelButton();
	dialog.setRange(0, count);
	dialog.setProgress(0);
	dialog.show();
	qApp->processEvents();

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*it);
		SolverDefinitionGridAttribute* cond = item->condition();
		GridAttributeContainer* container = grid->gridRelatedCondition(cond->name());
		if (! container->mapped()) {
			// mapping is done only when it is not mapped by the grid creating condition.
			item->executeMapping(grid, &dialog);
		}
	}
	dialog.hide();

	grid->setModified();
	PreProcessorGridDataItem* griddi = dynamic_cast<PreProcessorGridDataItem*>(conditiondi->gridDataItem());
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(griddi->nodeGroupDataItem())->updateActorSettings();
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(griddi->cellGroupDataItem())->updateActorSettings();
	renderGraphicsView();
}

void PreProcessorGridAttributeMappingSettingTopDataItem::setDefaultValues()
{
	// Get the grid.
	PreProcessorGridAndGridCreatingConditionDataItem* conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	Grid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*it);
		SolverDefinitionGridAttribute* cond = item->condition();
		GridAttributeContainer* container = grid->gridRelatedCondition(cond->name());
		if (! container->mapped()) {
			item->setDefaultValue(grid);
		}
	}
}

bool PreProcessorGridAttributeMappingSettingTopDataItem::checkDimensions()
{
	for (auto c_it = m_childItems.begin(); c_it != m_childItems.end(); ++c_it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*c_it);
		PreProcessorGeoDataGroupDataItem* gItem = item->geodataGroupDataItem();
		const QList<GridAttributeDimensionContainer*>& conts = gItem->dimensions()->containers();
		for (int i = 0; i < conts.size(); ++i) {
			GridAttributeDimensionContainer* c = conts.at(i);
			if (c->count() == 0) {
				QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Dimension %1 of geographic data %2 has no value. Can not start mapping.").arg(c->definition()->caption()).arg(gItem->condition()->caption()));
				return false;
			}
		}
	}
	return true;
}

void PreProcessorGridAttributeMappingSettingTopDataItem::customMapping(bool nomessage)
{
	// Get the grid.
	PreProcessorGridAndGridCreatingConditionDataItem* conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	Grid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}

	if (! checkDimensions()) {return;}
	PreProcessorGridAttributeCustomMappingDialog mappingDialog(mainWindow());
	QList<PreProcessorGridAttributeMappingSettingDataItem*> gridAtts;
	for (auto c_it = m_childItems.begin(); c_it != m_childItems.end(); ++c_it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*c_it);
		gridAtts.append(item);
	}

	QList<PreProcessorBCSettingDataItem*> bcs;
	const QList<GraphicsWindowDataItem*>& bclist = conditiondi->bcSettingGroupDataItem()->childItems();
	for (auto c_it = bclist.begin(); c_it != bclist.end(); ++c_it) {
		PreProcessorBCSettingDataItem* bcsdi = dynamic_cast<PreProcessorBCSettingDataItem*>(*c_it);
		if (bcsdi->bcDataItem()->hideSetting()) {continue;}
		bcs.append(bcsdi);
	}

	mappingDialog.setSettings(gridAtts, bcs, m_mappingSetting);
	int ret = mappingDialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_mappingSetting = mappingDialog.setting();

	int count = 0;

	for (int i = 0; i < gridAtts.size(); ++i) {
		PreProcessorGridAttributeMappingSettingDataItem* item = gridAtts.at(i);
		if (!m_mappingSetting.attSettings.value(item->condition()->name())) {
			continue;
		}
		GridAttributeContainer* cont = grid->gridRelatedCondition(item->condition()->name());
		if (cont->isCustomModified()) {
			int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The grid attribute \"%1\" is edited by hand. When you execute mapping, all modifications you made will be discarted. Do you really want to execute mapping?").arg(item->condition()->caption()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::Yes) {
				count = count + item->mappingCount();
			} else {
				m_mappingSetting.attSettings.remove(item->condition()->name());
				m_mappingSetting.attSettings.insert(item->condition()->name(), false);
			}
		} else {
			count = count + item->mappingCount();
		}
	}

	for (int i = 0; i < bcs.size(); ++i) {
		PreProcessorBCSettingDataItem* item = bcs.at(i);
		if (!m_mappingSetting.bcSettings.value(item->bcDataItem()->uniqueName())) {
			continue;
		}
		if (item->bcDataItem()->isCustomModified()) {
			int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The boundary condition \"%1\" is edited by hand. When you execute mapping, all modifications you made will be discarted. Do you really want to execute mapping?").arg(item->bcDataItem()->standardItem()->text()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::Yes) {
				count = count + 1;
			} else {
				m_mappingSetting.bcSettings.remove(item->bcDataItem()->uniqueName());
				m_mappingSetting.bcSettings.insert(item->bcDataItem()->uniqueName(), false);
			}
		} else {
			count = count + 1;
		}
	}

	WaitDialog dialog(preProcessorWindow());
	dialog.showProgressBar();
	dialog.disableCancelButton();
	dialog.setRange(0, count);
	dialog.setProgress(0);
	dialog.show();
	qApp->processEvents();

	for (int i = 0; i < gridAtts.size(); ++i) {
		PreProcessorGridAttributeMappingSettingDataItem* item = gridAtts.at(i);
		if (! m_mappingSetting.attSettings.value(item->condition()->name())) {
			continue;
		}
		GridAttributeContainer* cont = grid->gridRelatedCondition(item->condition()->name());
		item->executeMapping(grid, &dialog);
		cont->setCustomModified(false);
	}

	for (int i = 0; i < bcs.size(); ++i) {
		PreProcessorBCSettingDataItem* item = bcs.at(i);
		if (! m_mappingSetting.bcSettings.value(item->bcDataItem()->uniqueName())) {
			continue;
		}
		item->executeMapping(true, &dialog);
	}
	dialog.hide();

	grid->setModified();
	PreProcessorGridDataItem* griddi = dynamic_cast<PreProcessorGridDataItem*>(conditiondi->gridDataItem());
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(griddi->nodeGroupDataItem())->updateActorSettings();
	renderGraphicsView();
	if (! nomessage) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("Mapping geographic data finished successfully."));
	}
}

void PreProcessorGridAttributeMappingSettingTopDataItem::customMapping(const std::string& attName, bool nomessage)
{
	// Get the grid.
	PreProcessorGridAndGridCreatingConditionDataItem* conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	Grid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	if (grid->gridRelatedCondition(attName)->isCustomModified()) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The grid attribute \"%1\" is edited by hand. When you execute mapping, all modifications you made will be discarted. Do you really want to execute mapping?").arg(grid->gridRelatedCondition(attName)->condition()->caption()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*it);
		if (item->condition() == grid->gridRelatedCondition(attName)->condition()) {
			// execute mapping.
			item->executeMapping(grid, 0);
			grid->gridRelatedCondition(attName)->setCustomModified(false);
		}
	}
	grid->setModified();
	if (! nomessage) {
		QMessageBox::information(mainWindow(), tr("Mapping geographic data finished"), tr("Mapping geographic data finished successfully."));
	}
}
