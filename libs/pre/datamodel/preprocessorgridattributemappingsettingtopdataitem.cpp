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

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

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
	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	SolverDefinitionGridType* gType = gtItem->gridType();
	const auto& conditions = gType->gridAttributes();
	for (auto cond : conditions) {
		auto geodataGroup = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gtItem->geoDataTop()->groupDataItem(cond->name()));
		auto item = new PreProcessorGridAttributeMappingSettingDataItem(cond, geodataGroup, this);
		m_itemNameMap.insert(cond->name(), item);
		m_childItems.push_back(item);
	}
	const auto& cconditions = gType->gridComplexAttributes();
	for (auto cond : cconditions) {
		auto geodataGroup = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gtItem->geoDataTop()->groupDataItem(cond->name()));
		auto item = new PreProcessorGridAttributeMappingSettingDataItem(cond, geodataGroup, this);
		m_itemNameMap.insert(cond->name(), item);
		m_childItems.push_back(item);
	}
}

void PreProcessorGridAttributeMappingSettingTopDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void PreProcessorGridAttributeMappingSettingTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}

void PreProcessorGridAttributeMappingSettingTopDataItem::informGeoDataChange()
{}

void PreProcessorGridAttributeMappingSettingTopDataItem::executeMapping()
{
	// Get the grid.
	auto conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	v4InputGrid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	if (! checkDimensions()) {return;}
	int count = 0;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(child);
		SolverDefinitionGridAttribute* cond = item->condition();
		auto container = grid->attribute(cond->name());
		if (! container->mapped()) {
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

	QStringList skippedAtts;

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*it);
		SolverDefinitionGridAttribute* cond = item->condition();
		auto container = grid->attribute(cond->name());
		if (! container->mapped()) {
			// mapping is done only when it is not mapped by the grid creating condition.
			item->executeMapping(grid, &dialog);
		} else {
			if (item->geodataGroupDataItem()->childItems().size() > 1) {
				// the group has child items other than default value.
				skippedAtts.push_back(item->geodataGroupDataItem()->condition()->caption());
			}
		}
	}
	dialog.hide();

	grid->setIsModified(true);
	auto griddi = dynamic_cast<PreProcessorGridDataItem*>(conditiondi->gridDataItem());
	dynamic_cast<PreProcessorGridAttributeNodeGroupDataItem*>(griddi->nodeGroupDataItem())->updateActorSetting();
	dynamic_cast<PreProcessorGridAttributeCellGroupDataItem*>(griddi->cellGroupDataItem())->updateActorSetting();

	if (skippedAtts.size() > 0) {
		QString msg = tr("The following attributes were not mapped, because grid generator output values for them. "
										 "If you want to map geographic data for them forcibly, please map manually with menu "
										 "\"Grid\" -> \"Attributes Mapping\" -> \"Execute\".\n");
		for (auto a : skippedAtts) {
			msg += "* " + a + "\n";
		}
		QMessageBox::information(mainWindow(), tr("Information"), msg);
	}

	renderGraphicsView();
}

void PreProcessorGridAttributeMappingSettingTopDataItem::setDefaultValues()
{
	// Get the grid.
	PreProcessorGridAndGridCreatingConditionDataItem* conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	v4InputGrid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(child);
		SolverDefinitionGridAttribute* cond = item->condition();
		auto container = grid->attribute(cond->name());
		if (! container->mapped()) {
			item->setDefaultValue(grid);
		}
	}
}

bool PreProcessorGridAttributeMappingSettingTopDataItem::checkDimensions()
{
	return true;
/*
	for (auto c_it = m_childItems.begin(); c_it != m_childItems.end(); ++c_it) {
		PreProcessorGridAttributeMappingSettingDataItem* item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(*c_it);
		PreProcessorGeoDataGroupDataItem* gItem = item->geodataGroupDataItem();
		const auto& conts = gItem->dimensions()->containers();
		for (int i = 0; i < conts.size(); ++i) {
			GridAttributeDimensionContainer* c = conts.at(i);
			if (c->count() == 0) {
				QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Dimension %1 of geographic data %2 has no value. Can not start mapping.").arg(c->definition()->caption()).arg(gItem->condition()->caption()));
				return false;
			}
		}
	}
	return true;
*/
}

void PreProcessorGridAttributeMappingSettingTopDataItem::customMapping(bool nomessage)
{
	// Get the grid.
	auto conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	v4InputGrid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	iRICMainWindowI* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}

	if (! checkDimensions()) {return;}
	PreProcessorGridAttributeCustomMappingDialog mappingDialog(mainWindow());
	QList<PreProcessorGridAttributeMappingSettingDataItem*> gridAtts;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(child);
		gridAtts.append(item);
	}

	QList<PreProcessorBCSettingDataItem*> bcs;
	auto bcSettingGroup = conditiondi->bcSettingGroupDataItem();
	if (bcSettingGroup != nullptr) {
		for (auto item : bcSettingGroup->childItems()) {
			auto bcsdi = dynamic_cast<PreProcessorBCSettingDataItem*>(item);
			if (bcsdi->bcDataItem()->hideSetting()) {continue;}
			bcs.append(bcsdi);
		}
	}

	mappingDialog.setSettings(gridAtts, bcs, m_mappingSetting);
	int ret = mappingDialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_mappingSetting = mappingDialog.setting();

	int count = 0;

	for (int i = 0; i < gridAtts.size(); ++i) {
		auto item = gridAtts.at(i);
		if (!m_mappingSetting.attSettings.value(item->condition()->name())) {
			continue;
		}
		auto att = grid->attribute(item->condition()->name());
		if (att->isCustomModified()) {
			int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The grid attribute \"%1\" is edited by hand. When you execute mapping, all modifications you made will be discarded. Do you really want to execute mapping?").arg(item->condition()->caption()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
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
			int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The boundary condition \"%1\" is edited by hand. When you execute mapping, all modifications you made will be discarded. Do you really want to execute mapping?").arg(item->bcDataItem()->standardItem()->text()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
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
		auto att = grid->attribute(item->condition()->name());
		item->executeMapping(grid, &dialog);
		att->setCustomModified(false);
	}

	for (int i = 0; i < bcs.size(); ++i) {
		PreProcessorBCSettingDataItem* item = bcs.at(i);
		if (! m_mappingSetting.bcSettings.value(item->bcDataItem()->uniqueName())) {
			continue;
		}
		item->executeMapping(true, &dialog);
	}
	dialog.hide();

	grid->setIsModified(true);
	auto griddi = dynamic_cast<PreProcessorGridDataItem*>(conditiondi->gridDataItem());
	griddi->updateSimplifiedGrid();
	renderGraphicsView();
	if (! nomessage) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("Mapping geographic data finished successfully."));
	}
}

void PreProcessorGridAttributeMappingSettingTopDataItem::customMapping(const std::string& attName, bool nomessage)
{
	// Get the grid.
	PreProcessorGridAndGridCreatingConditionDataItem* conditiondi = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	v4InputGrid* grid = conditiondi->gridDataItem()->grid();
	if (grid == nullptr) {return;}
	if (grid->attribute(attName)->isCustomModified() && ! iricMainWindow()->cuiMode()) {
		int ret = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The grid attribute \"%1\" is edited by hand. When you execute mapping, all modifications you made will be discarded. Do you really want to execute mapping?").arg(grid->attribute(attName)->gridAttribute()->caption()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	for (auto child : m_childItems) {
		auto item = dynamic_cast<PreProcessorGridAttributeMappingSettingDataItem*>(child);
		if (item->condition() == grid->attribute(attName)->gridAttribute()) {
			// execute mapping.
			item->executeMapping(grid, 0);
			grid->attribute(attName)->setCustomModified(false);
		}
	}
	grid->setIsModified(true);
	if (! nomessage && ! iricMainWindow()->cuiMode()) {
		QMessageBox::information(mainWindow(), tr("Mapping geographic data finished"), tr("Mapping geographic data finished successfully."));
	}
}
