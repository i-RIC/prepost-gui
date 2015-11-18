#define _USE_MATH_DEFINES
#include "datamodel/preprocessorbackgroundimagedataitem.h"
#include "datamodel/preprocessorbcdataitem.h"
#include "datamodel/preprocessorbcgroupdataitem.h"
#include "datamodel/preprocessorbcsettingdataitem.h"
#include "datamodel/preprocessorbcsettinggroupdataitem.h"
#include "datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include "datamodel/preprocessorgridattributecelldataitem.h"
#include "datamodel/preprocessorgridattributemappingsettingtopdataitem.h"
#include "datamodel/preprocessorgridattributenodedataitem.h"
#include "datamodel/preprocessorgridcreatingconditiondataitem.h"
#include "datamodel/preprocessorgriddataitem.h"
#include "datamodel/preprocessorgridtypedataitem.h"
#include "datamodel/preprocessorinputconditiondataitem.h"
#include "datamodel/preprocessormeasureddatatopdataitem.h"
#include "datamodel/preprocessorgeodatacomplexgroupdataitem.h"
#include "datamodel/preprocessorgeodatadataitem.h"
#include "datamodel/preprocessorgeodatadataitem.h"
#include "datamodel/preprocessorgeodatagroupdataitem.h"
#include "datamodel/preprocessorgeodatatopdataitem.h"
#include "datamodel/preprocessorrootdataitem.h"
#include "factory/hydraulicdataimporterfactory.h"
#include "factory/geodatafactory.h"
#include "preobjectbrowserview.h"
#include "preprocessordatamodel.h"
#include "preprocessorgraphicsview.h"
#include "preprocessorwindow.h"

#include <guibase/itemselectingdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/measureddata/measureddatafiledataitem.h>
#include <guicore/measureddata/measureddatapointgroupdataitem.h>
#include <guicore/measureddata/measureddatavectorgroupdataitem.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>
#include <guicore/project/cgnsfilelist.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/project/projectworkspace.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/filesystemfunction.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/mathsupport.h>
#include <geodata/pointmap/geodatapointmap.h>
#include <geodata/polygon/geodatapolygon.h>
#include <geodata/polygon/geodatapolygoncreator.h>
#include <geodata/riversurvey/geodatariversurvey.h>

#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QGraphicsItemGroup>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QVector2D>

#include <vtkRenderWindow.h>

#include <cmath>

PreProcessorDataModel::PreProcessorDataModel(PreProcessorWindow* w, ProjectDataItem* parent)
	: PreProcessorDataModelInterface(w, parent)
{
	m_objectBrowserView = nullptr;
	init();
	setupAdditinalMenus();
}

PreProcessorDataModel::~PreProcessorDataModel()
{
	m_objectBrowserView->blockSignals(true);
	delete m_rootDataItem;
	m_objectBrowserView->blockSignals(false);
}

PreProcessorGraphicsViewInterface* PreProcessorDataModel::graphicsView() const
{
	return dynamic_cast<PreProcessorGraphicsViewInterface*>(m_graphicsView);
}

void PreProcessorDataModel::init()
{
	// setup the basic itemModel structure.
	PreProcessorRootDataItem* root = new PreProcessorRootDataItem(dynamic_cast<PreProcessorWindow*>(m_mainWindow), this);
	m_rootDataItem = root;
	m_geoDataAddSignalMapper = nullptr;
	root->setZDepthRange(m_dataRange);
	root->setupStandardModel(m_itemModel);
	connect(m_itemModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleObjectBrowserChange(QStandardItem*)));
	connect(this, SIGNAL(additionalMenusUpdated(QList<QMenu*>)), m_mainWindow, SLOT(handleAdditionalMenusUpdate(QList<QMenu*>)));

	m_graphicsView->setActiveDataItem(root);
	m_graphicsView->setModel(this);

	m_mouseBoundingBox = new MouseBoundingBox(dynamic_cast<VTK2DGraphicsView*>(m_graphicsView), this);
	m_mouseBoundingBox->setZDepth(m_hoverRange.max());
}

bool PreProcessorDataModel::importInputCondition(const QString& filename)
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	return root->m_inputConditionDataItem->importInputCondition(filename);
}

bool PreProcessorDataModel::exportInputCondition(const QString& filename)
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	return root->m_inputConditionDataItem->exportInputCondition(filename);
}

bool PreProcessorDataModel::isInputConditionSet()
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	return root->m_inputConditionDataItem->isSet();
}

void PreProcessorDataModel::showCalcConditionDialog()
{
	iRICMainWindowInterface* mainW = iricMainWindow();
	bool solverRunning = mainW->isSolverRunning();
	if (solverRunning) {
		int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("The solver is running. You can see the calculation condition settings, but can not overwrite."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
		if (ret == QMessageBox::Cancel) {return;}
	}
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	root->m_inputConditionDataItem->showDialog(solverRunning);
}

void PreProcessorDataModel::importCalcCondition()
{
	QString selectedFilter;
	QString fname = QFileDialog::getOpenFileName(
		iricMainWindow(), tr("Select file to import"), LastIODirectory::get(), tr("iRIC project file (*.ipro);;CGNS file (*.cgn);;YAML file (*.yml)"), &selectedFilter);
	if (fname == "") {return;}
	if (selectedFilter == tr("iRIC project file (*.ipro)")) {
		importCalcConditionFromOtherProject(fname);
	} else if (selectedFilter == tr("CGNS file (*.cgn)")) {
		importCalcConditionFromCGNS(fname);
	} else if (selectedFilter == tr("YAML file (*.yml)")) {
		importCalcConditionFromYaml(fname);
	} else {
		// invalid!
		Q_ASSERT(0);
	}
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

void PreProcessorDataModel::importCalcConditionFromOtherProject(const QString& fname)
{
	QFileInfo finfo(fname);
	// load the project data.
	ProjectWorkspace* w = projectData()->mainWindow()->workspace();
	QString tmpWorkfolder = ProjectData::newWorkfolderName(w->workspace());
	ProjectData tmpProj(tmpWorkfolder, nullptr);
	tmpProj.unzipFrom(fname);
	tmpProj.loadCgnsList();

	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(projectData()->mainWindow()->preProcessorWindow());
	// now it's loaded. find how many cgns files are included.
	QList<CgnsFileList::CgnsFileEntry*> list = tmpProj.mainfile()->cgnsFileList()->cgnsFiles();
	if (list.count() == 1) {
		// automatically use the only cgns file.
		QString fullname = tmpProj.workCgnsFileName(list.first()->filename());
		bool ret = pre->importInputCondition(fullname);
		if (! ret) {
			// not imported.
			goto ERROR;
		}
	} else {
		QStringList items;
		for (auto it = list.begin(); it != list.end(); ++it) {
			items << (*it)->filename();
		}
		QFileInfo tmpp(fname);
		QString projname = tmpp.fileName();
		bool ok;
		QString solname = QInputDialog::getItem(projectData()->mainWindow(), tr("Select case"), tr("Please select from which case in %1 to import calculation conditions.").arg(projname), items, 0, false, &ok);
		if (! ok) {goto ERROR;}
		QString fullname = tmpProj.workCgnsFileName(solname);
		bool ret = pre->importInputCondition(fullname);
		if (! ret) {
			// not imported.
			goto ERROR;
		}
	}
	iRIC::rmdirRecursively(tmpWorkfolder);
	QMessageBox::information(projectData()->mainWindow(), tr("Success"), tr("Calculation Condition is successfully imported from the specified file."));
	LastIODirectory::set(finfo.absolutePath());
	setModified();
	return;

ERROR:
	iRIC::rmdirRecursively(tmpWorkfolder);
}

void PreProcessorDataModel::importCalcConditionFromCGNS(const QString& fname)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(projectData()->mainWindow()->preProcessorWindow());
	if (pre->importInputCondition(fname)) {
		QMessageBox::information(projectData()->mainWindow(), tr("Success"), tr("Calculation Condition is successfully imported from %1.").arg(QDir::toNativeSeparators(fname)));
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
		setModified();
	} else {
		QMessageBox::critical(projectData()->mainWindow(), tr("Fail"), tr("Importing calculation condition failed."));
	}
}

void PreProcessorDataModel::importCalcConditionFromYaml(const QString& fname)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(projectData()->mainWindow()->preProcessorWindow());
	if (pre->importInputCondition(fname)){
		QMessageBox::information(projectData()->mainWindow(), tr("Success"), tr("Calculation Condition is successfully imported from the specified file."));
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
		setModified();
	} else {
		QMessageBox::critical(projectData()->mainWindow(), tr("Fail"), tr("Importing calculation condition failed."));
	}
}

void PreProcessorDataModel::exportCalcCondition()
{
	QString fname = QFileDialog::getSaveFileName(
										projectData()->mainWindow(), tr("Select File to Export"), LastIODirectory::get(), tr("CGNS file (*.cgn)")
									);
	if (fname == "") {return;}
	iRICMainWindowInterface* mainW = projectData()->mainWindow();
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(mainW->preProcessorWindow());
	mainW->statusBar()->showMessage(tr("Exporting calculation condition..."));
	if (pre->exportInputCondition(fname)) {
		mainW->statusBar()->showMessage(tr("Calculation Condition is successfully exported to %1.").arg(QDir::toNativeSeparators(fname)), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
		QFileInfo finfo(fname);
		LastIODirectory::set(finfo.absolutePath());
	} else {
		mainW->statusBar()->clearMessage();
		QMessageBox::critical(mainW, tr("Fail"), tr("Exporting calculation condition failed."));
	}
}

void PreProcessorDataModel::setupAdditinalMenus()
{
	// delete old dummy menus.
	m_oldDummyMenusList.append(m_dummyMenus);
	m_dummyMenus.clear();
	m_additionalMenus.clear();
	// dummy menus!
	setupGeoDataMenus();
	setupGridMenu();
	setupMeasuredValuesMenu();

	// remove old menus.
	while (m_oldDummyMenusList.count() > OLDMENULIST_MAX) {
		QList<QMenu*> oldDummyMenus = m_oldDummyMenusList.at(0);
		m_oldDummyMenusList.pop_front();
		for (auto it = oldDummyMenus.begin(); it != oldDummyMenus.end(); ++it) {
			delete *it;
		}
	}
	emit additionalMenusUpdated(m_additionalMenus);
}

void PreProcessorDataModel::setupGeoDataMenus()
{
	m_geographicDataMenu = new QMenu(tr("G&eographic Data"), mainWindow());
	m_additionalMenus.append(m_geographicDataMenu);
	m_dummyMenus.append(m_geographicDataMenu);

	GeoDataCreator* polygonCreator = nullptr;
	PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(m_selectedItem);

	QAction* editGroupAction = new QAction(tr("Edit &Groups..."), m_geographicDataMenu);
	editGroupAction->setDisabled(true);
	QAction* deleteAllAction = new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), tr("Delete &All..."), m_geographicDataMenu);
	QAction* exportAllPolygonsAction = new QAction(QIcon(":/libs/guibase/images/iconExport.png"), tr("Export All Polygons..."), m_geographicDataMenu);

	m_geographicDataMenu->addAction(editGroupAction);
	m_geographicDataMenu->addSeparator();

	if (item != nullptr) {
		// GeoData dataitem is selected.
		QList<GeoDataCreator*> creators = GeoDataFactory::instance().compatibleCreators(dynamic_cast<PreProcessorGeoDataGroupDataItem*>(item->parent())->condition());
		for (auto it = creators.begin(); it != creators.end(); ++it) {
			if (dynamic_cast<GeoDataPolygonCreator*>(*it) != nullptr) {
				polygonCreator = *it;
			}
		}
		setupGeoDataAddActions(dynamic_cast<PreProcessorGeoDataGroupDataItem*>(item->parent()));
		QMenu* dummy;

		GeoData* raw = item->geoData();
		GeoDataRiverSurvey* s = dynamic_cast<GeoDataRiverSurvey*>(raw);
		dummy = new QMenu(tr("&River Survey"), m_geographicDataMenu);
		if (s == nullptr) {
			dummy->setDisabled(true);
		} else {
			dummy->addActions(s->menu()->actions());
		}
		m_geographicDataMenu->addMenu(dummy);

		GeoDataPointmap* pm = dynamic_cast<GeoDataPointmap*>(raw);
		dummy = new QMenu(tr("P&ointset Data"), m_geographicDataMenu);
		if (pm == nullptr) {
			dummy->setDisabled(true);
		} else {
			dummy->addActions(pm->menu()->actions());
		}
		m_geographicDataMenu->addMenu(dummy);

		GeoDataPolygon* pol = dynamic_cast<GeoDataPolygon*>(raw);
		dummy = new QMenu(tr("&Polygon"), m_geographicDataMenu);
		dummy->addAction(m_geoDataAddActions.value(polygonCreator));
		if (pol != nullptr) {
			dummy->addSeparator();
			dummy->addActions(pol->menu()->actions());
		}
		m_geographicDataMenu->addMenu(dummy);
		deleteAllAction->setDisabled(true);
		exportAllPolygonsAction->setDisabled(true);
	} else {
		PreProcessorGeoDataGroupDataItem* gitem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(m_selectedItem);
		if (gitem != nullptr) {
			// GeoDatagroup dataitem is selected.
			QList<GeoDataCreator*> creators = GeoDataFactory::instance().compatibleCreators(gitem->condition());
			for (auto it = creators.begin(); it != creators.end(); ++it) {
				if (dynamic_cast<GeoDataPolygonCreator*>(*it) != nullptr) {
					polygonCreator = *it;
				}
			}
			setupGeoDataAddActions(gitem);
			QMenu* dummy;
			dummy = new QMenu(tr("&River Survey"), m_geographicDataMenu);
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			dummy = new QMenu(tr("P&ointset Data"), m_geographicDataMenu);
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			dummy = new QMenu(tr("&Polygon"), m_geographicDataMenu);
			dummy->addAction(m_geoDataAddActions.value(polygonCreator));
			m_geographicDataMenu->addMenu(dummy);

			PreProcessorGeoDataComplexGroupDataItem* cgitem = dynamic_cast<PreProcessorGeoDataComplexGroupDataItem*>(gitem);
			if (cgitem != nullptr) {
				editGroupAction->setEnabled(true);
				connect(editGroupAction, SIGNAL(triggered()), cgitem, SLOT(showEditGroupDialog()));
			}
			connect(deleteAllAction, SIGNAL(triggered()), gitem, SLOT(deleteAll()));
			exportAllPolygonsAction->setEnabled(gitem->polygonExists());
			connect(exportAllPolygonsAction, SIGNAL(triggered()), gitem, SLOT(exportAllPolygons()));
		} else {
			// no raw data selected. create dummy menus.
			QMenu* dummy;
			dummy = new QMenu(tr("&River Survey"), m_geographicDataMenu);
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			dummy = new QMenu(tr("P&ointset Data"), mainWindow());
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			dummy = new QMenu(tr("&Polygon"), mainWindow());
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			deleteAllAction->setDisabled(true);
			exportAllPolygonsAction->setDisabled(true);
		}
	}
	m_geographicDataMenu->addSeparator();

	m_geographicDataMenu->addAction(deleteAllAction);
	m_geographicDataMenu->addSeparator();

	QMenu* colorMapMenu = new QMenu(tr("&Color Setting..."), m_geographicDataMenu);
	colorMapMenu->setIcon(QIcon(":/libs/guibase/images/iconColor.png"));
	m_geographicDataMenu->addMenu(colorMapMenu);
	QAction* setupScalarBarAction = new QAction(tr("Set Up Scalarbar..."), m_geographicDataMenu);
	m_geographicDataMenu->addAction(setupScalarBarAction);

	// add colormap edit menu for each raw data type.
	PreProcessorGridTypeDataItem* gti = getGridTypeItem(m_selectedItem);
	if (gti != nullptr) {
		QList<PreProcessorGeoDataGroupDataItemInterface*> groups = gti->geoDataTop()->groupDataItems();
		for (auto it = groups.begin(); it != groups.end(); ++it) {
			PreProcessorGeoDataGroupDataItemInterface* groupDataitem = *it;
			QString condCaption = groupDataitem->condition()->caption();
			condCaption.append("...");
			QAction* action = new QAction(condCaption, colorMapMenu);
			connect(action, SIGNAL(triggered()), groupDataitem, SLOT(editScalarsToColors()));
			colorMapMenu->addAction(action);
		}
		connect(setupScalarBarAction, SIGNAL(triggered()), gti->geoDataTop(), SLOT(setupScalarBar()));
	} else {
		colorMapMenu->setDisabled(true);
		setupScalarBarAction->setDisabled(true);
	}
	m_geographicDataMenu->addSeparator();

	PreProcessorWindow* preWindow = dynamic_cast<PreProcessorWindow*>(mainWindow());
	QMenu* importMenu = new QMenu(tr("Import"), m_geographicDataMenu);
	importMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	m_geographicDataMenu->addMenu(importMenu);
	connect(importMenu, SIGNAL(aboutToShow()), preWindow, SLOT(setupGeoDataImportMenu()));
	QMenu* exportMenu = new QMenu(tr("Export"), m_geographicDataMenu);
	exportMenu->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	m_geographicDataMenu->addMenu(exportMenu);
	connect(exportMenu, SIGNAL(aboutToShow()), preWindow, SLOT(setupGeoDataExportMenu()));

	m_geographicDataMenu->addAction(exportAllPolygonsAction);
}

QMenu* PreProcessorDataModel::setupGridCreationMenu(QMenu* parentMenu, PreProcessorGridCreatingConditionDataItemInterface* /*gcItem*/)
{
	QMenu* gridCreationMenu = new QMenu(tr("&Grid Creating Condition"), parentMenu);
	connect(gridCreationMenu, SIGNAL(aboutToShow()), this, SLOT(setupGridCreationMenuContent()));
	return gridCreationMenu;
}

QMenu* PreProcessorDataModel::setupBoundaryConditionSettingMenu(QMenu* parentMenu)
{
	QMenu* bcsMenu = new QMenu(tr("B&oundary Condition Setting"), parentMenu);
	connect(bcsMenu, SIGNAL(aboutToShow()), this, SLOT(setupBoundaryConditionSettingMenuContent()));
	return bcsMenu;
}

void PreProcessorDataModel::setupGridMenu()
{
	// Add Grid menu.
	// check whether grid or child of grid is selected.
	PreProcessorGridDataItem* gItem = getGridItem(m_selectedItem);
	m_gridMenu = new QMenu(tr("&Grid"), mainWindow());
	if (gItem != nullptr) {
		// ----------------------------
		// grid creating related menus.
		// ----------------------------

		PreProcessorGridCreatingConditionDataItemInterface* gcItem = nullptr;
		gcItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(gItem->parent())->creatingConditionDataItem();
		m_gridMenu->addAction(gcItem->switchAction());
		// grid creating condition menu
		QMenu* creationMenu = setupGridCreationMenu(m_gridMenu, gcItem);
		m_gridMenu->addMenu(creationMenu);
		if (gItem->bcGroupDataItem() != nullptr) {
			QMenu* bcsMenu = setupBoundaryConditionSettingMenu(m_gridMenu);
			m_gridMenu->addMenu(bcsMenu);
		}
		m_gridMenu->addAction(gcItem->createAction());
		m_gridMenu->addSeparator();

		// ----------------------------
		// add mapping menu
		// ----------------------------

		QMenu* mappingMenu = new QMenu(tr("&Attributes Mapping"), m_gridMenu);
		m_gridMenu->addMenu(mappingMenu);
		mappingMenu->addAction(gItem->mappingAction());
		PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(rootDataItem());
		mappingMenu->addAction(rItem->editGridAttributeMappingSettingAction());

		m_gridMenu->addSeparator();

		// ----------------------------
		// add grid menu items
		// ----------------------------

		m_gridMenu->addActions(gItem->menu()->actions());

		// disable node related menus when node dataitem is not selected currently.
		PreProcessorGridAttributeNodeDataItem* nItem =
			dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(m_selectedItem);
		gItem->setNodeDataItem(nItem);
		if (nItem != nullptr) {
			gItem->nodeEditAction()->setEnabled(gItem->selectedVertices().count() > 0);
			gItem->nodeEditAction()->disconnect();
			connect(gItem->nodeEditAction(), SIGNAL(triggered()), nItem, SLOT(editValue()));
			gItem->nodeDisplaySettingAction()->setEnabled(true);
			gItem->nodeDisplaySettingAction()->disconnect();
			connect(gItem->nodeDisplaySettingAction(), SIGNAL(triggered()), nItem, SLOT(showDisplaySettingDialog()));
		} else {
			gItem->nodeEditAction()->setEnabled(false);
			gItem->nodeEditAction()->disconnect();
			gItem->nodeDisplaySettingAction()->setEnabled(false);
		}
		// disable cell related menus when cell dataitem is not selected currently.
		PreProcessorGridAttributeCellDataItem* cItem =
			dynamic_cast<PreProcessorGridAttributeCellDataItem*>(m_selectedItem);
		gItem->setCellDataItem(cItem);
		if (cItem != nullptr) {
			gItem->cellEditAction()->setEnabled(gItem->selectedCells().count() > 0);
			gItem->cellEditAction()->disconnect();
			connect(gItem->cellEditAction(), SIGNAL(triggered()), cItem, SLOT(editValue()));
			gItem->cellDisplaySettingAction()->setEnabled(true);
			gItem->cellDisplaySettingAction()->disconnect();
			connect(gItem->cellDisplaySettingAction(), SIGNAL(triggered()), cItem, SLOT(showDisplaySettingDialog()));
		} else {
			gItem->cellEditAction()->setEnabled(false);
			gItem->cellEditAction()->disconnect();
			gItem->cellDisplaySettingAction()->setEnabled(false);
		}

		PreProcessorBCDataItem* bcItem =
			dynamic_cast<PreProcessorBCDataItem*>(m_selectedItem);
		if (gItem->bcGroupDataItem() != nullptr) {
			gItem->bcGroupDataItem()->bcMenu()->setDisabled(gItem->grid() == nullptr);
			gItem->bcGroupDataItem()->updateBCMenu(bcItem);
		}

		m_gridMenu->addAction(gItem->birdEyeWindowAction());
		m_gridMenu->addSeparator();
		PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(gItem->parent()->parent());
		m_gridMenu->addAction(gtItem->addNewGridAction());

		m_gridMenu->addSeparator();
		// ----------------------------
		// add import menu
		// ----------------------------
		m_gridMenu->addAction(gItem->importAction());

		// ----------------------------
		// add export menu
		// ----------------------------
		m_gridMenu->addAction(gItem->exportAction());
	} else {
		// Active item is not grid or grid child nodes.
//		m_gridMenu->setDisabled(true);
	}
	m_additionalMenus.append(m_gridMenu);
	m_dummyMenus.append(m_gridMenu);
}

void PreProcessorDataModel::setupMeasuredValuesMenu()
{
	MeasuredDataFileDataItem* fitem = getMeasuredDataItem(m_selectedItem);
	m_measuredValuesMenu = new QMenu(tr("&Measured Values"));
	QAction* pointAction = m_measuredValuesMenu->addAction(tr("&Scalar..."));
	QAction* vectorAction = m_measuredValuesMenu->addAction(tr("&Arrows..."));
	m_measuredValuesMenu->addSeparator();
	QAction* importAction = m_measuredValuesMenu->addAction(QIcon(":/libs/guibase/images/iconImport.png"), tr("&Import..."));
	connect(importAction, SIGNAL(triggered()), iricMainWindow(), SLOT(importMeasuredData()));

	if (fitem == nullptr) {
		pointAction->setDisabled(true);
		vectorAction->setDisabled(true);
	} else {
		connect(pointAction, SIGNAL(triggered()), fitem->pointGroupDataItem(), SLOT(showSettingDialog()));
		connect(vectorAction, SIGNAL(triggered()), fitem->vectorGroupDataItem(), SLOT(showSettingDialog()));
	}
	m_additionalMenus.append(m_measuredValuesMenu);
	m_dummyMenus.append(m_measuredValuesMenu);
}

PreProcessorGridTypeDataItem* PreProcessorDataModel::getGridTypeItem(GraphicsWindowDataItem* item)
{
	if (item == nullptr) {return nullptr;}
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(item);
	if (r != nullptr) {return nullptr;}
	PreProcessorGridTypeDataItem* g = dynamic_cast<PreProcessorGridTypeDataItem*>(item);
	if (g != nullptr) {return g;}
	return getGridTypeItem(dynamic_cast<GraphicsWindowDataItem*>(item->parent()));
}

PreProcessorGridDataItem* PreProcessorDataModel::getGridItem(GraphicsWindowDataItem* item)
{
	PreProcessorGridDataItem* gItem = getGridItemRecursively(item);
	if (gItem != nullptr) {return gItem;}
	// try to find the gridtype node that contains the current item.
	PreProcessorGridTypeDataItem* gtItem = getGridTypeItem(item);
	if (gtItem != nullptr) {
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gtItem->conditions();
		if (conds.count() > 0) {
			// select the first one.
			PreProcessorGridAndGridCreatingConditionDataItemInterface* gagcItem = conds[0];
			return dynamic_cast<PreProcessorGridDataItem*>(gagcItem->gridDataItem());
		}
	}
	// try to find some grid dataitem in one of the gridtypes.
	PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	for (auto gtIt = gtItems.begin(); gtIt != gtItems.end(); ++gtIt) {
		PreProcessorGridTypeDataItem* gtItem = *gtIt;
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gtItem->conditions();
		if (conds.count() > 0) {
			// select the first one.
			PreProcessorGridAndGridCreatingConditionDataItemInterface* gagcItem = conds[0];
			return dynamic_cast<PreProcessorGridDataItem*>(gagcItem->gridDataItem());
		}
	}
	return nullptr;
}

PreProcessorGridDataItem* PreProcessorDataModel::getGridItemRecursively(GraphicsWindowDataItem* item)
{
	if (item == nullptr) {return nullptr;}
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(item);
	if (r != nullptr) {
		// reached root data item.
		return nullptr;
	}
	PreProcessorGridDataItem* g = dynamic_cast<PreProcessorGridDataItem*>(item);
	if (g != nullptr) {return g;}
	PreProcessorGridAndGridCreatingConditionDataItem* a = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(item);
	if (a != nullptr) {
		return dynamic_cast<PreProcessorGridDataItem*>(a->gridDataItem());
	}
	return getGridItemRecursively(dynamic_cast<GraphicsWindowDataItem*>(item->parent()));
}

MeasuredDataFileDataItem* PreProcessorDataModel::getMeasuredDataItem(GraphicsWindowDataItem* item)
{
	MeasuredDataFileDataItem* fItem = getMeasuredDataItemRecursively(item);
	if (fItem != nullptr) {return fItem;}
	// if there is only one Measured data, use that.
	PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	PreProcessorMeasuredDataTopDataItem* mtItem = rItem->measuredDataTopDataItem();
	if (mtItem->childItems().count() == 1) {
		MeasuredDataFileDataItem* ret = dynamic_cast<MeasuredDataFileDataItem*>(mtItem->childItems().at(0));
		return ret;
	}
	return nullptr;
}

MeasuredDataFileDataItem* PreProcessorDataModel::getMeasuredDataItemRecursively(GraphicsWindowDataItem* item)
{
	if (item == nullptr) {return nullptr;}
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(item);
	if (r != nullptr) {
		// reached root data item.
		return nullptr;
	}
	MeasuredDataFileDataItem* f = dynamic_cast<MeasuredDataFileDataItem*>(item);
	if (f != nullptr) {return f;}
	return getMeasuredDataItemRecursively(dynamic_cast<GraphicsWindowDataItem*>(item->parent()));
}

PreProcessorBackgroundImageDataItem* PreProcessorDataModel::getBackgroundImageItem(GraphicsWindowDataItem* item)
{
	if (item == nullptr) {return nullptr;}
	PreProcessorBackgroundImageDataItem* b = dynamic_cast<PreProcessorBackgroundImageDataItem*>(item);
	return b;
}

PreProcessorBCSettingDataItem* PreProcessorDataModel::getBCSettingDataItem(GraphicsWindowDataItem* item)
{
	PreProcessorBCSettingDataItem* bcitem = dynamic_cast<PreProcessorBCSettingDataItem*>(item);
	return bcitem;
}


void PreProcessorDataModel::addGridImportMenu(QMenu* menu)
{
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool importAvailable = false;
	if (gridTypes.count() == 0) {
		// no menu available.
	} else if (gridTypes.count() == 1) {
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		importAvailable = addGridImportMenuForGridType(menu, gt, true);
	} else {
		for (int i = 0; i < gridTypes.count(); ++i) {
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (addGridImportMenuForGridType(gtMenu, gt, false)) {
				importAvailable = true;
			} else {
				delete gtMenu;
			}
		}
	}
	if (! importAvailable) {
		QAction* no = menu->addAction(tr("&Grid..."));
		no->setDisabled(true);
	}
}

bool PreProcessorDataModel::addGridImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone)
{
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gt->conditions();
	if (conds.count() == 0) {
		// no menu available.
		return false;
	} else if (conds.count() == 1) {
		PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(0);
		if (alone) {
			// this is the only existing grid.
			if (! addGridImportMenuForGrid(menu, di, tr("&Grid"))) {
				return false;
			}
		} else {
			if (! addGridImportMenuForGrid(menu, di, di->caption())) {
				return false;
			}
		}
		return true;
	} else {
		// there are multiple grids.
		bool okExists = false;
		QMenu* tMenu = menu->addMenu(tr("&Grid"));
		for (int i = 0; i < conds.count(); ++i) {
			PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(i);
			QMenu* gMenu = tMenu->addMenu(di->caption());
			if (addGridImportMenuForGrid(gMenu, di, di->caption())) {
				okExists = true;
			} else {
				delete gMenu;
			}
		}
		if (! okExists) {
			delete tMenu;
		}
		return okExists;
	}
}

bool PreProcessorDataModel::addGridImportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name)
{
	QString cap = QString("%1...").arg(name);
	QAction* action = new QAction(cap, menu);

	PreProcessorGridAndGridCreatingConditionDataItem* dii =
			dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (di);
	connect(action, SIGNAL(triggered()), dii, SLOT(importGrid()));
	menu->addAction(action);
	return true;
}

void PreProcessorDataModel::addGridExportMenu(QMenu* menu)
{
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool exportAvailable = false;
	if (gridTypes.count() == 0) {
		// no menu available.
	} else if (gridTypes.count() == 1) {
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		exportAvailable = addGridExportMenuForGridType(menu, gt, true);
	} else {
		for (int i = 0; i < gridTypes.count(); ++i) {
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (addGridExportMenuForGridType(gtMenu, gt, false)) {
				exportAvailable = true;
			} else {
				delete gtMenu;
			}
		}
	}
	if (! exportAvailable) {
		QAction* no = menu->addAction(tr("&Grid..."));
		no->setDisabled(true);
	}
}

bool PreProcessorDataModel::addGridExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone)
{
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gt->conditions();
	if (conds.count() == 0) {
		// no menu available.
		return false;
	} else if (conds.count() == 1) {
		PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(0);
		if (alone) {
			// this is the only existing grid.
			if (! addGridExportMenuForGrid(menu, di, tr("&Grid"))) {
				return false;
			}
		} else {
			if (! addGridExportMenuForGrid(menu, di, di->caption())) {
				return false;
			}
		}
		return true;
	} else {
		// there are multiple grids.
		bool okExists = false;
		QMenu* tMenu = menu->addMenu(tr("&Grid"));
		for (int i = 0; i < conds.count(); ++i) {
			PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(i);
			QMenu* gMenu = tMenu->addMenu(di->caption());
			if (addGridExportMenuForGrid(gMenu, di, di->caption())) {
				okExists = true;
			} else {
				delete gMenu;
			}
		}
		if (! okExists) {
			delete tMenu;
		}
		return okExists;
	}
}

bool PreProcessorDataModel::addGridExportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name)
{
	QString cap = QString("%1...").arg(name);
	PreProcessorGridDataItem* gdi = dynamic_cast<PreProcessorGridDataItem*>(di->gridDataItem());
	if (! gdi->isExportAvailable()) {return false;}
	QAction* action = new QAction(cap, menu);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), gdi, SLOT(exportGrid()));
	return true;
}

void PreProcessorDataModel::setupGeoDataImportMenu(QMenu* menu)
{
	// clear menu first.
	menu->clear();
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool importAvailable = false;
	if (gridTypes.count() == 0) {
		// no menu available.
	} else if (gridTypes.count() == 1) {
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		importAvailable = setupGeoDataImportMenuForGridType(menu, gt);
	} else {
		for (int i = 0; i < gridTypes.count(); ++i) {
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (setupGeoDataImportMenuForGridType(gtMenu, gt)) {
				importAvailable = true;
			} else {
				delete gtMenu;
			}
		}
	}
	if (! importAvailable) {
		QAction* no = menu->addAction(tr("(No data to import)"));
		no->setDisabled(true);
	}
}


void PreProcessorDataModel::setupHydraulicDataImportMenu(QMenu* menu)
{
	// clear menu first.
	menu->clear();

	QList<HydraulicDataImporter*> importers = HydraulicDataImporterFactory::instance().importers();
	for (auto it = importers.begin(); it != importers.end(); ++it) {
		HydraulicDataImporter* imp = *it;
		QAction* action = menu->addAction(imp->caption());
		connect(action, SIGNAL(triggered()), this, SLOT(importHydraulicData()));
	}
}

bool PreProcessorDataModel::setupGeoDataImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt)
{
	PreProcessorGeoDataTopDataItemInterface* topItem = gt->geoDataTop();
	QList<PreProcessorGeoDataGroupDataItemInterface*> groups = topItem->groupDataItems();
	if (groups.count() == 0) {
		// no menu available.
		return false;
	} else {
		// there are multiple grids.
		bool okExists = false;
		for (int i = 0; i < groups.count(); ++i) {
			PreProcessorGeoDataGroupDataItemInterface* gi = groups.at(i);
			setupGeoDataImportMenuForGroup(menu, gi);
			okExists = true;
		}
		return okExists;
	}
}

bool PreProcessorDataModel::setupGeoDataImportMenuForGroup(QMenu* menu, PreProcessorGeoDataGroupDataItemInterface* gt)
{
	PreProcessorGeoDataGroupDataItem* item = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gt);
	item->addImportAction(menu);
	return true;
}

void PreProcessorDataModel::setupGeoDataExportMenu(QMenu* menu)
{
	// clear menu first.
	menu->clear();
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool importAvailable = false;
	if (gridTypes.count() == 0) {
		// no menu available.
	} else if (gridTypes.count() == 1) {
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		importAvailable = setupGeoDataExportMenuForGridType(menu, gt);
	} else {
		for (int i = 0; i < gridTypes.count(); ++i) {
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (setupGeoDataExportMenuForGridType(gtMenu, gt)) {
				importAvailable = true;
			} else {
				delete gtMenu;
			}
		}
	}
	if (! importAvailable) {
		QAction* no = menu->addAction(tr("(No data to export)"));
		no->setDisabled(true);
	}
}

bool PreProcessorDataModel::setupGeoDataExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt)
{
	PreProcessorGeoDataTopDataItemInterface* topItem = gt->geoDataTop();
	QList<PreProcessorGeoDataGroupDataItemInterface*> groups = topItem->groupDataItems();
	if (groups.count() == 0) {
		// no menu available.
		return false;
	} else {
		for (int i = 0; i < groups.count(); ++i) {
			PreProcessorGeoDataGroupDataItemInterface* gi = groups.at(i);
			QString cap = gi->condition()->caption();
			cap.append("...");
			QAction* exportAction = menu->addAction(cap);
			exportAction->setEnabled(geoDataExportAvailable(gi));
			connect(exportAction, SIGNAL(triggered()), gi, SLOT(doExport()));
		}
	}
	return true;
}

bool PreProcessorDataModel::setupGeoDataExportMenuForGroup(QMenu* menu, PreProcessorGeoDataGroupDataItem* gt)
{
	bool ok = false;
	QList<PreProcessorGeoDataDataItemInterface*> datas = gt->geoDatas();
	for (int i = 0; i < datas.count(); ++i) {
		PreProcessorGeoDataDataItemInterface* item = datas.at(i);
		QMenu* tmpmenu = menu->addMenu(item->standardItem()->text());
		if (setupGeoDataExportMenuForItem(tmpmenu, item)) {
			ok = true;
		} else {
			delete tmpmenu;
		}
	}
	return ok;
}

bool PreProcessorDataModel::geoDataExportAvailable(PreProcessorGeoDataGroupDataItemInterface* gt)
{
	QList<PreProcessorGeoDataDataItemInterface*> datas = gt->geoDatas();
	for (int i = 0; i < datas.count(); ++i) {
		PreProcessorGeoDataDataItemInterface* item = datas.at(i);
		if (item->geoData()->exporters().count() > 0) {
			return true;
		}
	}
	return false;
}

bool PreProcessorDataModel::setupGeoDataExportMenuForItem(QMenu* menu, PreProcessorGeoDataDataItemInterface* gt)
{
	return dynamic_cast<PreProcessorGeoDataDataItem*>(gt)->setupExportMenu(menu);
}

void PreProcessorDataModel::setupGridCreationMenuContent()
{
	QMenu* gridCreationMenu = dynamic_cast<QMenu*>(sender());
	gridCreationMenu->clear();
	// Add Grid Creation menu.
	PreProcessorGridDataItem* gItem = getGridItem(m_selectedItem);
	if (gItem == nullptr) {return;}
	PreProcessorGridCreatingConditionDataItemInterface* gcItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(gItem->parent())->creatingConditionDataItem();
	if (gcItem == nullptr) {return;}
	QMenu* dummy = dynamic_cast<PreProcessorGridCreatingConditionDataItem*>(gcItem)->menu();
	if (dummy == nullptr) {
		QAction* dummyAction = gridCreationMenu->addAction(tr("(No algorithm is selected yet)"));
		dummyAction->setDisabled(true);
	} else {
		gridCreationMenu->addActions(dummy->actions());

		gridCreationMenu->addSeparator();
		gridCreationMenu->addAction(gcItem->clearAction());
	}
}

void PreProcessorDataModel::setupBoundaryConditionSettingMenuContent()
{
	QMenu* bcsMenu = dynamic_cast<QMenu*>(sender());
	bcsMenu->clear();
	PreProcessorGridDataItem* gItem = getGridItem(m_selectedItem);
	if (gItem == nullptr) {return;}
	PreProcessorBCSettingGroupDataItem* bcsgItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(gItem->parent())
			->bcSettingGroupDataItem();
	PreProcessorBCSettingDataItem* bcItem = getBCSettingDataItem(m_selectedItem);
	if (bcItem != nullptr) {
		bcsMenu->addAction(bcItem->editAction());
	} else {
		bcsMenu->addAction(bcsgItem->dummyEditAction());
	}
	bcsMenu->addSeparator();
	const QList<QAction*>& addActions = bcsgItem->addActions();
	for (auto it = addActions.begin(); it != addActions.end(); ++it) {
		bcsMenu->addAction(*it);
	}
	bcsMenu->addSeparator();
	if (bcItem != nullptr) {
		bcsMenu->addAction(bcItem->deleteAction());
	} else {
		bcsMenu->addAction(bcsgItem->dummyDeleteAction());
	}
}

void PreProcessorDataModel::disableGridMenu()
{
	m_gridMenu->setDisabled(true);
}

void PreProcessorDataModel::setupGeoDataAddActions(PreProcessorGeoDataGroupDataItem* item)
{
	if (m_geoDataAddSignalMapper != nullptr) {delete m_geoDataAddSignalMapper;}
	m_geoDataAddSignalMapper = new QSignalMapper(this);
	for (auto it = m_geoDataAddActions.begin(); it != m_geoDataAddActions.end(); ++it) {
		delete *it;
	}
	m_geoDataAddActions.clear();
	QList<GeoDataCreator*> creators = GeoDataFactory::instance().compatibleCreators(item->condition());
	for (auto cit = creators.begin(); cit != creators.end(); ++cit) {
		QAction* action = new QAction(tr("Add New %1").arg((*cit)->caption()), this);
		m_geoDataAddActions.insert(*cit, action);
		m_geoDataAddSignalMapper->setMapping(action, *cit);
		connect(action, SIGNAL(triggered()), m_geoDataAddSignalMapper, SLOT(map()));
	}
	connect(m_geoDataAddSignalMapper, SIGNAL(mapped(QObject*)), item, SLOT(addGeoData(QObject*)));
}

bool PreProcessorDataModel::gridEdited()
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	return root->gridEdited();
}

void PreProcessorDataModel::toggleGridEditFlag()
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	root->toggleGridEditFlag();
}

void PreProcessorDataModel::informUnfocusRiverCrosssectionWindows()
{
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gtItems = r->gridTypeDataItems();
	for (auto it = gtItems.begin(); it != gtItems.end(); ++it) {
		PreProcessorGridTypeDataItem* gtItem = *it;
		QList<PreProcessorGeoDataGroupDataItemInterface*> gitems = gtItem->geoDataTop()->groupDataItems();
		for (auto it2 = gitems.begin(); it2 != gitems.end(); ++it2) {
			PreProcessorGeoDataGroupDataItemInterface* gItem = *it2;
			QList<PreProcessorGeoDataDataItemInterface*> geodatas = gItem->geoDatas();
			for (auto it3 = geodatas.begin(); it3 != geodatas.end(); ++it3) {
				PreProcessorGeoDataDataItemInterface* dItem = *it3;
				GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(dItem->geoData());
				if (rs != nullptr) {
					rs->setColoredPoints(0, 0, 0);
				}
			}
		}
	}
}

bool PreProcessorDataModel::isSetupCorrectly() const
{
	PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	return rItem->inputConditionDataItem()->isSetupCorrectly();
}

bool PreProcessorDataModel::checkMappingStatus()
{
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gtItems = r->gridTypeDataItems();
	bool mapExexuted = false;
	for (auto it = gtItems.begin(); it != gtItems.end(); ++it) {
		QStringList notMapped;
		PreProcessorGridTypeDataItem* gtItem = *it;
		QList<PreProcessorGeoDataGroupDataItemInterface*> gitems = gtItem->geoDataTop()->groupDataItems();
		QList<PreProcessorGeoDataGroupDataItemInterface*> groupsToMap;
		for (auto it2 = gitems.begin(); it2 != gitems.end(); ++it2) {
			PreProcessorGeoDataGroupDataItemInterface* gItem = *it2;
			QStringList geodatasNotMapped = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gItem)->getGeoDatasNotMapped();
			if (geodatasNotMapped.count() > 0) {
				groupsToMap.append(gItem);
				notMapped.append(geodatasNotMapped);
			}
		}
		if (notMapped.count() > 0) {
			// geodata in this grid type is not mapped.
			int ret = QMessageBox::warning(m_mainWindow, PreProcessorDataModel::tr("Warning"), PreProcessorDataModel::tr("%1 are not mapped after they are edited last time. Do you want to execute mapping now?").arg(notMapped.join(", ")), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
			if (ret == QMessageBox::Yes) {
				// execute mapping for each grids.
				const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions = gtItem->conditions();
				for (auto cit = conditions.begin(); cit != conditions.end(); ++cit) {
					PreProcessorGridAndGridCreatingConditionDataItemInterface* cond = *cit;
					PreProcessorGridAndGridCreatingConditionDataItem* cond2 = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(cond);
					PreProcessorGridAttributeMappingSettingTopDataItem* tItem = cond2->mappingSettingDataItem();
					for (int i = 0; i < groupsToMap.count(); ++i) {
						PreProcessorGeoDataGroupDataItemInterface* gItem = groupsToMap[i];
						tItem->customMapping(gItem->condition()->name(), true);
					}
				}
				mapExexuted = true;
			} else if (ret == QMessageBox::Cancel) {
				return false;
			}
		}

		const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions = gtItem->conditions();
		for (auto cit = conditions.begin(); cit != conditions.end(); ++cit) {
			PreProcessorGridAndGridCreatingConditionDataItem* cond = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(*cit);
			const QList <GraphicsWindowDataItem*>& bcslist = cond->bcSettingGroupDataItem()->childItems();
			for (auto cit = bcslist.begin(); cit != bcslist.end(); ++cit) {
				PreProcessorBCSettingDataItem* bcsdi = dynamic_cast<PreProcessorBCSettingDataItem*>(*cit);
				if (! bcsdi->isMapped()) {
					int ret = QMessageBox::warning(m_mainWindow, PreProcessorDataModel::tr("Warning"), PreProcessorDataModel::tr("Boundary Condition Setting \"%1\" is not mapped after it is edited last time. Do you want to execute mapping now?").arg(bcsdi->bcDataItem()->caption()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
					if (ret == QMessageBox::Yes) {
						bcsdi->executeMapping(true, 0);
						mapExexuted = true;
					} else if (ret == QMessageBox::Cancel) {
						return false;
					}
				}
			}
		}
	}


	if (mapExexuted) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("Mapping geographic data, boundary condition finished successfully."));
	}
	return ! mapExexuted;
}

void PreProcessorDataModel::importHydraulicData()
{
	QAction* action = dynamic_cast<QAction*>(sender());

	QList<HydraulicDataImporter*> importers = HydraulicDataImporterFactory::instance().importers();
	HydraulicDataImporter* importer = nullptr;
	for (auto it = importers.begin(); it != importers.end(); ++it) {
		HydraulicDataImporter* imp = *it;
		if (imp->caption() == action->text()) {
			importer = imp;
			break;
		}
	}
	if (importer == nullptr) {return;}
	// scan all geodata.
	QList<GeoData*> geodatas;

	PreProcessorRootDataItem* ritem = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> titems = ritem->gridTypeDataItems();
	for (auto tit = titems.begin(); tit != titems.end(); ++tit) {
		PreProcessorGridTypeDataItem* titem = *tit;
		QList<PreProcessorGeoDataGroupDataItemInterface*> gitems = titem->geoDataTop()->groupDataItems();
		for (auto git = gitems.begin(); git != gitems.end(); ++git) {
			PreProcessorGeoDataGroupDataItemInterface* gitem = *git;
			QList<PreProcessorGeoDataDataItemInterface*> ditems = gitem->geoDatas();
			for (auto dit = ditems.begin(); dit != ditems.end(); ++dit) {
				PreProcessorGeoDataDataItemInterface* ditem = *dit;
				GeoData* rdata = ditem->geoData();
				if (importer->canImportTo(rdata)) {
					// this importer can used for this raw data.
					geodatas.append(rdata);
				}
			}
		}
	}
	GeoData* targetGeoData = nullptr;
	if (geodatas.count() == 0) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("There is no geographic data to import this hydraulic data."));
		return;
	} else if (geodatas.count() > 1) {
		ItemSelectingDialog dialog(mainWindow());
		QList<QString> items;
		for (int i = 0; i < geodatas.count(); ++i) {
			items.append(geodatas.at(i)->caption());
		}
		dialog.setItems(items);
		dialog.setWindowTitle(tr("Select geographic data"));
		dialog.setMessage(tr("Please select the geographic data to import hydraulic data."));
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {return;}
		targetGeoData = geodatas.at(dialog.selectIndex());
	} else {
		// there was only one geodata.
		targetGeoData = geodatas.at(0);
	}
	// get filename.
	QString dir = LastIODirectory::get();
	QString selectedFilter;
	// Select the file to import.
	QStringList filters = importer->fileDialogFilters();
	QString filename = QFileDialog::getOpenFileName(mainWindow(), tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()) {return;}
	bool result = importer->import(targetGeoData, filename, selectedFilter, mainWindow());
	if (result) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("Importing %1 succeeded.").arg(QDir::toNativeSeparators(filename)));
		QFileInfo finfo(filename);
		LastIODirectory::set(finfo.absolutePath());
	} else {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Importing %1 failed.").arg(QDir::toNativeSeparators(filename)));
	}
}

PreProcessorGeoDataTopDataItemInterface* PreProcessorDataModel::geoDataTopDataItem(const QString& type) const
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	PreProcessorGridTypeDataItem* tItem = root->gridTypeDataItem(type);
	if (tItem == nullptr) {return nullptr;}
	return tItem->geoDataTop();
}

PreProcessorGridAndGridCreatingConditionDataItemInterface* PreProcessorDataModel::getGridAndGridCreatingConditionDataItem(const QString& typeName, const QString& zoneName) const
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	return root->gridTypeDataItem(typeName)->condition(zoneName);
}

void PreProcessorDataModel::applyOffset(double x, double y)
{
	m_rootDataItem->applyOffset(x, y);
}
