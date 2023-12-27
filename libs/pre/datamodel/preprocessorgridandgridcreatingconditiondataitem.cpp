#include "../factory/gridimporterfactory.h"
#include "../factory/preprocessorgriddataitemfactory.h"
#include "../gridimporter/cgnsgridimporter.h"
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

#include <guicore/base/iricmainwindowi.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/grid/gridimporteri.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/grid/v4inputgridio.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/filesystemfunction.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

#include <typeinfo>

PreProcessorGridAndGridCreatingConditionDataItem::PreProcessorGridAndGridCreatingConditionDataItem(const std::string& zoneName, const QString& caption, PreProcessorDataItem* p) :
	PreProcessorGridAndGridCreatingConditionDataItemI {caption, p},
	m_caption {caption},
	m_zoneName (zoneName),
	m_bcSettingGroupDataItem {nullptr},
	m_bcGroupDataItem {nullptr},
	m_gridDataItem {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	QDir workdir(projectData()->workDirectory());
	setSubPath("gridandgridcreatingconditiondataitem");
	QDir subdir1(workdir.absoluteFilePath(relativeSubPath()));

	setSubPath(m_zoneName.c_str());
	QDir subdir2(workdir.absoluteFilePath(relativeSubPath()));

	if (subdir1.exists()) {
		if (subdir2.exists()) {
			// new folder and old folder both exists. remove the old folder.
			iRIC::rmdirRecursively(subdir1.absolutePath());
		} else {
			// old folder only exists. rename the old folder.
			subdir1.cdUp();
			subdir1.rename("gridandgridcreatingconditiondataitem", m_zoneName.c_str());
		}
	}

	auto gType = gridTypeDataItem()->gridType();

	m_creatingConditionDataItem = new PreProcessorGridCreatingConditionDataItem(this);
	m_childItems.push_back(m_creatingConditionDataItem);

	m_mappingSettingDataItem = new PreProcessorGridAttributeMappingSettingTopDataItem(this);
	m_childItems.push_back(m_mappingSettingDataItem);

	if (gType->boundaryConditions().size() > 0) {
		m_bcSettingGroupDataItem = new PreProcessorBCSettingGroupDataItem(this);
		m_childItems.push_back(m_bcSettingGroupDataItem);

		m_bcGroupDataItem = new PreProcessorBCGroupDataItem(this);
		connect(m_bcGroupDataItem, SIGNAL(itemsUpdated()), m_bcSettingGroupDataItem, SLOT(updateItems()));
		connect(m_bcGroupDataItem, SIGNAL(itemsLoaded()), m_bcSettingGroupDataItem, SLOT(loadItems()));
		m_standardItem->takeChild(m_bcGroupDataItem->standardItem()->row());
	}

	m_deleteAction = new QAction(PreProcessorGridAndGridCreatingConditionDataItem::tr("&Delete Grid Creating Condition and Grid Shape..."), this);
	m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.svg"));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteGridAndCondition()));

	// create connections.
	connect(m_creatingConditionDataItem, SIGNAL(gridCreated()), this, SLOT(informGridCreation()));
	connect(dynamic_cast<PreProcessorGridTypeDataItem*>(p)->geoDataTop(), SIGNAL(dataChanged()), m_mappingSettingDataItem, SLOT(informGeoDataChange()));

	setupGridDataItem(gType->emptyGrid());
}

const QString& PreProcessorGridAndGridCreatingConditionDataItem::caption() const
{
	return m_caption;
}

const std::string& PreProcessorGridAndGridCreatingConditionDataItem::zoneName() const
{
	return m_zoneName;
}

PreProcessorGridCreatingConditionDataItemI* PreProcessorGridAndGridCreatingConditionDataItem::creatingConditionDataItem() const
{
	return m_creatingConditionDataItem;
}

PreProcessorBCSettingGroupDataItem* PreProcessorGridAndGridCreatingConditionDataItem::bcSettingGroupDataItem() const
{
	return m_bcSettingGroupDataItem;
}

PreProcessorBCGroupDataItem* PreProcessorGridAndGridCreatingConditionDataItem::bcGroupDataItem() const
{
	return m_bcGroupDataItem;
}

PreProcessorGridAttributeMappingSettingTopDataItem* PreProcessorGridAndGridCreatingConditionDataItem::mappingSettingDataItem() const
{
	return m_mappingSettingDataItem;
}

void PreProcessorGridAndGridCreatingConditionDataItem::addCustomMenuItems(QMenu* menu)
{
	// add "Add Grid" menu.
	auto gtItem = gridTypeDataItem();
	gtItem->addCustomMenuItems(menu);
}

PreProcessorGridDataItemI* PreProcessorGridAndGridCreatingConditionDataItem::gridDataItem() const
{
	return m_gridDataItem;
}

bool PreProcessorGridAndGridCreatingConditionDataItem::isDeletable() const
{
	// Ask parent whether I am deletable.
	return gridTypeDataItem()->isChildDeletable(this);
}

void PreProcessorGridAndGridCreatingConditionDataItem::handleStandardItemChange()
{
	PreProcessorDataItem::handleStandardItemChange();
}

bool PreProcessorGridAndGridCreatingConditionDataItem::isGridEdited() const
{
	v4InputGrid* g = m_gridDataItem->grid();
	if (g == nullptr) {return m_gridDataItem->gridIsDeleted();}
	return g->isModified();
}

void PreProcessorGridAndGridCreatingConditionDataItem::setGridEdited()
{
	v4InputGrid* g = m_gridDataItem->grid();
	if (g == nullptr) {return;}

	g->setIsModified(true);
}

void PreProcessorGridAndGridCreatingConditionDataItem::setupGridDataItem(v4InputGrid* grid)
{
	if (grid == nullptr) {return;}

	auto gridItem = PreProcessorGridDataItemFactory::factory(grid, this);
	if (m_gridDataItem != nullptr && typeid(*m_gridDataItem).name() == typeid(*gridItem).name()) {
		delete gridItem;
		return;
	}

	if (m_gridDataItem != nullptr) {
		PreProcessorGridDataItemI* tmpItem = m_gridDataItem;
		tmpItem->unsetBCGroupDataItem();
		auto it = std::find(m_childItems.begin(), m_childItems.end(), tmpItem);
		if (it != m_childItems.end()) {m_childItems.erase(it);}
		m_gridDataItem = nullptr;
		updateItemMap();
		delete tmpItem;
	}

	m_gridDataItem = gridItem;
	m_gridDataItem->setBCGroupDataItem(m_bcGroupDataItem);
	m_childItems.push_back(gridItem);

	m_gridDataItem->updateActionStatus();

	// put the grid data item after grid creating condition
	m_standardItem->takeRow(gridItem->standardItem()->row());

	auto gtItem = gridTypeDataItem();
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
	if (targetParent != nullptr) {
		targetParent->insertRow(prevRow + 1, gItem);
	} else {
		// add as root node
		prevItem->model()->insertRow(prevRow + 1, gItem);
	}

	connect(gridItem, &PreProcessorGridDataItem::gridAttributeChanged, gtItem, &PreProcessorGridTypeDataItem::changeValueRange);

	updateItemMap();
	updateZDepthRange();
	qApp->processEvents();
}

int PreProcessorGridAndGridCreatingConditionDataItem::loadFromCgnsFile()
{
	auto mainFile = dataModel()->iricMainWindow()->projectData()->mainfile();
	auto zone = mainFile->cgnsFile()->ccBase()->zone(m_zoneName);
	if (zone == nullptr) {return IRIC_NO_ERROR;}

	auto gtItem = gridTypeDataItem();
	int ier;
	v4InputGrid* grid = v4InputGridIO::load(*zone, gtItem, subPath(), offset(), &ier);
	if (grid == nullptr) {return IRIC_INVALID_GRIDTYPE;}
	setupGridDataItem(grid);
	delete grid;

	return m_gridDataItem->loadFromCgnsFile();
}

void PreProcessorGridAndGridCreatingConditionDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_zoneName = iRIC::toStr(elem.attribute("zoneName"));
	// load grid creating condition information.
	QDomNode condNode = iRIC::getChildNode(node, "GridCreatingCondition");
	if (! condNode.isNull()) {m_creatingConditionDataItem->loadFromProjectMainFile(condNode);}
	// load boundary condition setting information.
	QDomNode bcNode = iRIC::getChildNode(node, "BoundaryConditionSettings");
	if (! bcNode.isNull() && m_bcSettingGroupDataItem != nullptr) {m_bcSettingGroupDataItem->loadFromProjectMainFile(bcNode);}
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

	writer.writeAttribute("zoneName", m_zoneName.c_str());
	writer.writeAttribute("caption", m_caption);

	// save grid creating condition information
	writer.writeStartElement("GridCreatingCondition");
	m_creatingConditionDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// save boundary condition setting information
	if (m_bcSettingGroupDataItem != nullptr) {
		writer.writeStartElement("BoundaryConditionSettings");
		m_bcSettingGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	// save grid information
	writer.writeStartElement("Grid");
	m_gridDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void PreProcessorGridAndGridCreatingConditionDataItem::saveExternalData(const QString&)
{}

void PreProcessorGridAndGridCreatingConditionDataItem::deleteGridAndCondition()
{
	iRICMainWindowI* mw = dataModel()->iricMainWindow();
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

bool PreProcessorGridAndGridCreatingConditionDataItem::importFromImporter(GridImporterI* importer, const QString& filename, const QString& selectedFilter)
{
	// create new empty grid.
	auto gt = importer->supportedGridType();
	v4InputGrid* importedGrid = gridTypeDataItem()->gridType()->createEmptyGrid(gt);
	setupGridDataItem(importedGrid);

	// now, import grid data.
	auto gridItem = dynamic_cast<PreProcessorGridDataItem*> (m_gridDataItem);
	importedGrid->setGridDataItem(gridItem);

	return m_gridDataItem->importFromImporter(importedGrid, importer, filename, selectedFilter);
}

PreProcessorGridTypeDataItem* PreProcessorGridAndGridCreatingConditionDataItem::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*> (parent());
}

void PreProcessorGridAndGridCreatingConditionDataItem::importGrid()
{
	iRICMainWindowI* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()){
		mw->warnSolverRunning();
		return;
	}
	QString dir = LastIODirectory::get();
	QSet<QString> filters;
	QMap<QString, QList<GridImporterI*> > importers;

	auto gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	auto importerList = GridImporterFactory::instance().list(*(gTypeItem->gridType()));

	for (auto importer : importerList) {
		for (auto filter : importer->fileDialogFilters()) {
			filters.insert(filter);
			if (! importers.contains(filter)) {
				QList<GridImporterI*> emptyList;
				importers.insert(filter, emptyList);
			}
			auto& imps = importers[filter];
			imps.append(importer);
		}
	}
	QRegExp re("\\((.+)\\)");
	QStringList filterList;
	QStringList prefixes;
	for (auto& filter : filters) {
		filterList.append(filter);
		if (re.indexIn(filter)) {
			prefixes.append(re.cap(1));
		}
	}
	// add "All importable files" in the front"
	prefixes.sort();
	filterList.insert(0, tr("All importable files (%1)").arg(prefixes.join(" ")));

	// Select the file to import.
	QString selectedFilter;
	QString filename = QFileDialog::getOpenFileName(projectData()->mainWindow(), tr("Select file to import"), dir, filterList.join(";;"), &selectedFilter);
	if (filename.isNull()){return;}
	QList<GridImporterI*> impsForFilter;
	if (importers.find(selectedFilter) != importers.end()) {
		impsForFilter = importers[selectedFilter];
	} else {
		// "All importable files" selected
		auto ext = QString("*.") + QFileInfo(filename).suffix();
		for (auto importer : importerList) {
			for (auto f : importer->fileDialogFilters()) {
				if (f.indexOf(ext) != -1) {
					impsForFilter.append(importer);
				}
			}
		}
	}
	bool ret = false;
	for (int i = 0; i < impsForFilter.size(); ++i) {
		auto importer = impsForFilter.at(i);
		ret = importFromImporter(importer, filename, selectedFilter);
		if (ret) {break;}
	}

	if (! ret){
		// import failed.
		m_gridDataItem->silentDeleteGrid();
		QMessageBox::critical(dataModel()->mainWindow(), tr("Error"), tr("Importing grid failed."));
		return;
	}

	// import succeeded.
	m_gridDataItem->grid()->setIsModified(true);

	QFileInfo finfo(filename);
	LastIODirectory::set(finfo.absolutePath());
	dataModel()->graphicsView()->cameraFit();

	mainWindow()->setFocus();
	m_gridDataItem->informGridChange();
}

bool PreProcessorGridAndGridCreatingConditionDataItem::importGridFromCgnsFile(const QString& filename)
{
	auto gTypeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	auto importerList = GridImporterFactory::instance().list(*(gTypeItem->gridType()));

	for (auto importer : importerList) {
		// try only CgnsImpoter
		auto cgnsImporter = dynamic_cast<CgnsGridImporter*> (importer);
		if (cgnsImporter == nullptr) {continue;}

		auto filters = cgnsImporter->fileDialogFilters();
		bool ok = importFromImporter(importer, filename, filters.first());
		if (ok) {
			dataModel()->graphicsView()->cameraFit();
			return true;
		}
	}
	return false;
}

void PreProcessorGridAndGridCreatingConditionDataItem::informGridCreation()
{
	// set default values first. this is done, always.
	m_mappingSettingDataItem->setDefaultValues();

	if (PreProcessorGridAttributeMappingMode::mode == PreProcessorGridAttributeMappingMode::mAuto) {
		int ret = QMessageBox::information(mainWindow(), tr("Confirmation"), tr("Do you want to map geographic data to grid attributes now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (ret == QMessageBox::Yes) {
			m_mappingSettingDataItem->executeMapping();
			if (m_bcSettingGroupDataItem != nullptr) {
				m_bcSettingGroupDataItem->executeMapping(true);
			}
		}
	}
	auto gItem = dynamic_cast<PreProcessorGridDataItem*>(m_gridDataItem);
	gItem->updateSimplifiedGrid(dataModel()->graphicsView());
	gItem->informGridAttributeChangeAll();
}
