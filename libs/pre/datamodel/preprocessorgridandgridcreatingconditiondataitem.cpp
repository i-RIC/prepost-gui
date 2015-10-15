#include "../gridimporter/cgnsgridimporter.h"
#include "../factory/gridimporterfactory.h"
#include "../factory/preprocessorgriddataitemfactory.h"
#include "../misc/preprocessorgridattributemappingmode.h"
#include "../preprocessorwindow.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorbcsettinggroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributemappingsettingtopdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorrootdataitem.h"
#include "preprocessorstructured2dgriddataitem.h"
#include "preprocessorunstructured2dgriddataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/grid/gridimporterinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/filesystemfunction.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDir>
#include <QDomElement>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

PreProcessorGridAndGridCreatingConditionDataItem::PreProcessorGridAndGridCreatingConditionDataItem(const QString& zonename, const QString& caption, PreProcessorDataItem* p) :
	PreProcessorGridAndGridCreatingConditionDataItemInterface {caption, p},
	m_zoneName {zonename},
	m_caption {caption},
	m_gridDataItem {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	QDir workdir(projectData()->workDirectory());
	setSubPath("gridandgridcreatingconditiondataitem");
	QDir subdir1(workdir.absoluteFilePath(relativeSubPath()));

	setSubPath(m_zoneName);
	QDir subdir2(workdir.absoluteFilePath(relativeSubPath()));

	if (subdir1.exists()) {
		if (subdir2.exists()) {
			// new folder and old folder both exists. remove the old folder.
			iRIC::rmdirRecursively(subdir1.absolutePath());
		} else {
			// old folder only exists. rename the old folder.
			subdir1.cdUp();
			subdir1.rename("gridandgridcreatingconditiondataitem", m_zoneName);
		}
	}

	m_creatingConditionDataItem = new PreProcessorGridCreatingConditionDataItem(this);
	m_childItems.append(m_creatingConditionDataItem);

	m_bcSettingGroupDataItem = new PreProcessorBCSettingGroupDataItem(this);
	m_childItems.append(m_bcSettingGroupDataItem);

	m_mappingSettingDataItem = new PreProcessorGridAttributeMappingSettingTopDataItem(this);
	m_childItems.append(m_mappingSettingDataItem);

	SolverDefinitionGridType* gType = dynamic_cast<PreProcessorGridTypeDataItem*>(parent())->gridType();

	if (gType->boundaryConditions().size() == 0) {
		m_standardItem->takeChild(m_bcSettingGroupDataItem->standardItem()->row());
	}

	m_deleteAction = new QAction(PreProcessorGridAndGridCreatingConditionDataItem::tr("&Delete Grid Creating Condition and Grid Shape..."), this);
	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteGridAndCondition()));

	// create connections.
	connect(m_creatingConditionDataItem, SIGNAL(gridCreated()), this, SLOT(informGridCreation()));
	connect(dynamic_cast<PreProcessorGridTypeDataItem*>(p)->geoDataTop(), SIGNAL(dataChanged()), m_mappingSettingDataItem, SLOT(informGeoDataChange()));

	setupGridDataItem(gType->emptyGrid());
}

void PreProcessorGridAndGridCreatingConditionDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_zoneName = elem.attribute("zoneName");
	// load grid creating condition information.
	QDomNode condNode = iRIC::getChildNode(node, "GridCreatingCondition");
	if (! condNode.isNull()) {m_creatingConditionDataItem->loadFromProjectMainFile(condNode);}
	// load boundary condition setting information.
	QDomNode bcNode = iRIC::getChildNode(node, "BoundaryConditionSetting");
	if (! bcNode.isNull()) {m_bcSettingGroupDataItem->loadFromProjectMainFile(bcNode);}
	// load grid information.
	QDomNode gridNode = iRIC::getChildNode(node, "Grid");
	if (! gridNode.isNull()) {m_gridDataItem->loadFromProjectMainFile(gridNode);}
}

void PreProcessorGridAndGridCreatingConditionDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_gridDataItem);
	if (gItem->bcGroupDataItem()) {
		gItem->bcGroupDataItem()->renumberItemsForProject();
	}

	writer.writeAttribute("zoneName", m_zoneName);
	writer.writeAttribute("caption", m_caption);

	// save grid creating condition information
	writer.writeStartElement("GridCreatingCondition");
	m_creatingConditionDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// save boundary condition setting information
	writer.writeStartElement("BoundaryConditionSettings");
	m_bcSettingGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// save grid information
	writer.writeStartElement("Grid");
	m_gridDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void PreProcessorGridAndGridCreatingConditionDataItem::saveExternalData(const QString&)
{

}

bool PreProcessorGridAndGridCreatingConditionDataItem::isDeletable() const
{
	// Ask parent whether I am deletable.
	return dynamic_cast<PreProcessorGridTypeDataItem*>(parent())->isChildDeletable(this);
}

void PreProcessorGridAndGridCreatingConditionDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
}

void PreProcessorGridAndGridCreatingConditionDataItem::addCustomMenuItems(QMenu* menu)
{
	// add "Add Grid" menu.
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	gtItem->addCustomMenuItems(menu);
}

bool PreProcessorGridAndGridCreatingConditionDataItem::gridEdited() const
{
	Grid* g = m_gridDataItem->grid();
	if (g == nullptr) {return false;}
	return g->isModified();
}

void PreProcessorGridAndGridCreatingConditionDataItem::toggleGridEditFlag()
{
	Grid* g = m_gridDataItem->grid();
	if (g == nullptr) {return;}
	g->setModified();
}

void PreProcessorGridAndGridCreatingConditionDataItem::setupGridDataItem(Grid* grid)
{
	if (m_gridDataItem != 0) {
		PreProcessorGridDataItemInterface* tmpItem = m_gridDataItem;
		m_childItems.removeOne(tmpItem);
		m_gridDataItem = 0;
		delete tmpItem;
	}

	PreProcessorGridDataItem* gridItem = PreProcessorGridDataItemFactory::factory(grid, this);
	m_gridDataItem = gridItem;
	m_childItems.append(gridItem);

	// put the grid data item after grid creating condition
	m_standardItem->takeRow(gridItem->standardItem()->row());

	PreProcessorGridTypeDataItem*	gtItem = dynamic_cast<PreProcessorGridTypeDataItem*> (parent());
	SolverDefinitionGridType* gt = gtItem->gridType();
	QStandardItem* prevItem = 0;
	if (gt->boundaryConditions().size() > 0) {
		prevItem = m_bcSettingGroupDataItem->standardItem();
	} else {
		prevItem = m_creatingConditionDataItem->standardItem();
	}
	QStandardItem* targetParent = prevItem->parent();
	QStandardItem* gItem = gridItem->standardItem();
	int prevRow = prevItem->row();
	if (targetParent != 0) {
		targetParent->insertRow(prevRow + 1, gItem);
	} else {
		// add as root node
		prevItem->model()->insertRow(prevRow + 1, gItem);
	}

	connect(gridItem, SIGNAL(gridRelatedConditionChanged(QString)), gtItem, SLOT(changeValueRange(QString)));
	if (gridItem->bcGroupDataItem() != 0){
		connect(gridItem->bcGroupDataItem(), SIGNAL(itemsUpdated()), m_bcSettingGroupDataItem, SLOT(updateItems()));
		connect(gridItem->bcGroupDataItem(), SIGNAL(itemsLoaded()), m_bcSettingGroupDataItem, SLOT(loadItems()));
	}
	updateItemMap();
}

void PreProcessorGridAndGridCreatingConditionDataItem::deleteGridAndCondition()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()){
		mw->warnSolverRunning();
		return;
	}
	if (QMessageBox::No == QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to discard grid creating condition and grid?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)){
		return;
	}
	auto cItem = dynamic_cast<PreProcessorGridCreatingConditionDataItem*> (m_creatingConditionDataItem);
	cItem->silentDeleteCondition();
	if (m_gridDataItem != 0) {
		m_gridDataItem->silentDeleteGrid();
	}
	iRICUndoStack::instance().clear();
}

void PreProcessorGridAndGridCreatingConditionDataItem::importGrid()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()){
		mw->warnSolverRunning();
		return;
	}
	QString dir = LastIODirectory::get();
	QString selectedFilter;
	QStringList filters;
	QList<GridImporterInterface*> importers;

	PreProcessorGridTypeDataItem* gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	QList<GridImporterInterface*> importerList = GridImporterFactory::instance().list(*(gTypeItem->gridType()));

	for (GridImporterInterface* iface : importerList) {
		QStringList flist = iface->fileDialogFilters();
		for (const QString& filter : flist) {
			filters.append(filter);
			importers.append(iface);
		}
	}

	// Select the file to import.
	QString filename = QFileDialog::getOpenFileName(projectData()->mainWindow(), tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()){return;}
	int index = filters.indexOf(selectedFilter);
	GridImporterInterface* importer = importers[index];

	// create new empty grid.
	SolverDefinitionGridType::GridType gt = importer->supportedGridType();
	Grid* importedGrid = dynamic_cast<PreProcessorGridTypeDataItem*>(parent())->gridType()->createEmptyGrid(gt);
	// set parent.
	importedGrid->setParent(this);
	// set zone name.
	importedGrid->setZoneName(zoneName());

	setupGridDataItem(importedGrid);

	// now, import grid data.
	bool ret = true;
	CgnsGridImporter* cgnsImpoter = dynamic_cast<CgnsGridImporter*> (importer);

	auto gridItem = dynamic_cast<PreProcessorGridDataItem*> (m_gridDataItem);
	if (cgnsImpoter != 0){
		// CGNS importer is a little special.
		// Boundary condition should be imported too.
		QString tmpname;
		int fn, B, zoneid;
		// create temporary CGNS file.
		bool internal_ret = cgnsImpoter->openCgnsFileForImporting(importedGrid, filename, tmpname, fn, B, zoneid, mainWindow());
		if (! internal_ret){goto IMPORT_ERROR_BEFORE_OPEN;}

		// load grid
		internal_ret = importedGrid->loadFromCgnsFile(fn, B, zoneid);
		if (! internal_ret){goto IMPORT_ERROR_AFTER_OPEN;}

		gridItem->setGrid(importedGrid);
		// import boundary condition
		if (gridItem->bcGroupDataItem() != 0) {
			gridItem->bcGroupDataItem()->clear();
			gridItem->bcGroupDataItem()->loadFromCgnsFile(fn);
		}
		cgnsImpoter->closeAndRemoveTempCgnsFile(fn, tmpname);
		goto IMPORT_SUCCEED;

IMPORT_ERROR_AFTER_OPEN:
		cgnsImpoter->closeAndRemoveTempCgnsFile(fn, tmpname);
IMPORT_ERROR_BEFORE_OPEN:
		ret = false;
IMPORT_SUCCEED:
		;
	} else {
		ret = importer->import(importedGrid, filename, selectedFilter, projectData()->mainWindow());
		if (ret) {
			gridItem->setGrid(importedGrid);
		}
	}

	if (! ret){
		// import failed.
		gridItem->silentDeleteGrid();
		QMessageBox::critical(dataModel()->mainWindow(), tr("Error"), tr("Importing grid failed."));
		return;
	}

	// import succeeded.
	importedGrid->setModified();

	QFileInfo finfo(filename);
	LastIODirectory::set(finfo.absolutePath());
	dataModel()->graphicsView()->cameraFit();

	mainWindow()->setFocus();
	gridItem->informGridChange();
}

void PreProcessorGridAndGridCreatingConditionDataItem::informGridCreation()
{
	// set default values first. this is done, always.
	m_mappingSettingDataItem->setDefaultValues();


	if (PreProcessorGridAttributeMappingMode::mode == PreProcessorGridAttributeMappingMode::mAuto) {
		int ret = QMessageBox::information(mainWindow(), tr("Confirmation"), tr("Do you want to map geographic data to grid attributes now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (ret == QMessageBox::Yes) {
			m_mappingSettingDataItem->executeMapping();
			m_bcSettingGroupDataItem->executeMapping(true);
		}
	}
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_gridDataItem);
	gItem->informgridRelatedConditionChangeAll();
}
