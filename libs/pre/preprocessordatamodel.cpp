#define _USE_MATH_DEFINES
#include <cmath>

#include <guicore/base/iricmainwindowinterface.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include "preprocessorwindow.h"
#include <guicore/misc/mouseboundingbox.h>
#include "datamodel/preprocessorrootdataitem.h"
#include "datamodel/preprocessorinputconditiondataitem.h"
#include "datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include "datamodel/preprocessorgriddataitem.h"
#include "datamodel/preprocessorrawdatatopdataitem.h"
#include "datamodel/preprocessorrawdatagroupdataitem.h"
#include "datamodel/preprocessorrawdatacomplexgroupdataitem.h"
#include "datamodel/preprocessorrawdatadataitem.h"
#include "datamodel/preprocessorgridcreatingconditiondataitem.h"
#include "datamodel/preprocessorgridrelatedconditionnodedataitem.h"
#include "datamodel/preprocessorgridrelatedconditioncelldataitem.h"
#include "datamodel/preprocessorbackgroundimagedataitem.h"
#include "datamodel/preprocessorgridattributemappingsettingtopdataitem.h"
#include "datamodel/preprocessorbcgroupdataitem.h"
#include "datamodel/preprocessorbcdataitem.h"
#include "datamodel/preprocessorbcsettinggroupdataitem.h"
#include "datamodel/preprocessorbcsettingdataitem.h"
#include <guicore/project/projectdata.h>
#include "preprocessordatamodel.h"
#include "preobjectbrowserview.h"
#include "preprocessorgraphicsview.h"
#include <guicore/project/projectmainfile.h>
#include "datamodel/preprocessorrawdatadataitem.h"
#include "datamodel/preprocessorgridtypedataitem.h"
#include <rawdata/pointmap/rawdatapointmap.h>
#include <rawdata/polygon/rawdatapolygon.h>
#include <rawdata/riversurvey/rawdatariversurvey.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include "factory/rawdatafactory.h"
#include <rawdata/polygon/rawdatapolygoncreator.h>
#include <guicore/measureddata/measureddatafiledataitem.h>
#include "datamodel/preprocessormeasureddatatopdataitem.h"
#include <guicore/measureddata/measureddatapointgroupdataitem.h>
#include <guicore/measureddata/measureddatavectorgroupdataitem.h>
#include "factory/hydraulicdataimporterfactory.h"
#include <guicore/pre/hydraulicdata/hydraulicdataimporter.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/project/projectworkspace.h>
#include <guicore/project/cgnsfilelist.h>

#include <guibase/itemselectingdialog.h>
#include <misc/lastiodirectory.h>

#include <QGraphicsItemGroup>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSignalMapper>
#include <QVector2D>
#include <QMenu>
#include <QApplication>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QStatusBar>

#include <vtkRenderWindow.h>

PreProcessorDataModel::PreProcessorDataModel(PreProcessorWindow* w, ProjectDataItem* parent)
	: PreProcessorDataModelInterface(w, parent)
{
	m_objectBrowserView = 0;
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
	m_rawDataAddSignalMapper = 0;
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
	if (solverRunning){
		int ret = QMessageBox::warning(mainWindow(), tr("Warning"), tr("The solver is running. You can see the calculation condition settings, but can not overwrite."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
		if (ret == QMessageBox::Cancel){return;}
	}
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	root->m_inputConditionDataItem->showDialog(solverRunning);
}

void PreProcessorDataModel::importCalcCondition()
{
	QString selectedFilter;
	QString fname = QFileDialog::getOpenFileName(
		iricMainWindow(), tr("Select file to import"), LastIODirectory::get(), tr("iRIC project file (*.ipro);;CGNS file (*.cgn)"), &selectedFilter
			);
	if (fname == ""){return;}
	if (selectedFilter == tr("iRIC project file (*.ipro)")){
		importCalcConditionFromOtherProject(fname);
	} else if (selectedFilter == tr("CGNS file (*.cgn)")){
		importCalcConditionFromCGNS(fname);
	} else {
		// invalid!
		Q_ASSERT(0);
	}
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

void PreProcessorDataModel::importCalcConditionFromOtherProject(const QString& fname)
{
	// load the project data.
	ProjectWorkspace* w = projectData()->mainWindow()->workspace();
	QString tmpworkfoler = ProjectData::newWorkfolderName(w->workspace());
	ProjectData tmpProj(tmpworkfoler, 0);
	tmpProj.open(fname);
	tmpProj.loadCgnsList();

	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (projectData()->mainWindow()->preProcessorWindow());
	// now it's loaded. find how many cgns files are included.
	QList<CgnsFileList::CgnsFileEntry*> list = tmpProj.mainfile()->cgnsFileList()->cgnsFiles();
	if (list.count() == 1){
		// automatically use the only cgns file.
		QString fullname = tmpProj.workCgnsFileName(list.first()->filename());
		bool ret = pre->importInputCondition(fullname);
		if (! ret){
			// not imported.
			return;
		}
	}else{
		QStringList items;
		QList<CgnsFileList::CgnsFileEntry*>::iterator it;
		for (it = list.begin(); it != list.end(); ++it){
			items << (*it)->filename();
		}
		QFileInfo tmpp(fname);
		QString projname = tmpp.fileName();
		bool ok;
		QString solname = QInputDialog::getItem(projectData()->mainWindow(), tr("Select case"), tr("Please select from which case in %1 to import calculation conditions.").arg(projname), items, 0, false, &ok);
		if (! ok){return;}
		QString fullname = tmpProj.workCgnsFileName(solname);
		bool ret = pre->importInputCondition(fullname);
		if (! ret){
			// not imported.
			return;
		}
	}
	QMessageBox::information(projectData()->mainWindow(), tr("Success"), tr("Calculation Condition is successfully imported from the specified file."));
	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
	setModified();
}

void PreProcessorDataModel::importCalcConditionFromCGNS(const QString& fname)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (projectData()->mainWindow()->preProcessorWindow());
	if (pre->importInputCondition(fname)){
		QMessageBox::information(projectData()->mainWindow(), tr("Success"), tr("Calculation Condition is successfully imported from %1.").arg(QDir::toNativeSeparators(fname)));
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
	if (fname == ""){return;}
	iRICMainWindowInterface* mainW = projectData()->mainWindow();
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (mainW->preProcessorWindow());
	mainW->statusBar()->showMessage(tr("Exporting calculation condition..."));
	if (pre->exportInputCondition(fname)){
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
	setupRawDataMenus();
	setupGridMenu();
	setupMeasuredValuesMenu();

	// remove old menus.
	while (m_oldDummyMenusList.count() > OLDMENULIST_MAX){
		QList<QMenu*> oldDummyMenus = m_oldDummyMenusList.at(0);
		m_oldDummyMenusList.pop_front();
		QList<QMenu*>::iterator it;
		for (it = oldDummyMenus.begin(); it != oldDummyMenus.end(); ++it){
			delete *it;
		}
	}
	emit additionalMenusUpdated(m_additionalMenus);
}

void PreProcessorDataModel::setupRawDataMenus()
{
	m_geographicDataMenu = new QMenu(tr("G&eographic Data"), mainWindow());
	m_additionalMenus.append(m_geographicDataMenu);
	m_dummyMenus.append(m_geographicDataMenu);

	RawDataCreator* polygonCreator = 0;
	PreProcessorRawdataDataItem* item = dynamic_cast<PreProcessorRawdataDataItem*>(m_selectedItem);

	QAction* editGroupAction = new QAction(tr("Edit &Groups..."), m_geographicDataMenu);
	editGroupAction->setDisabled(true);
	QAction* deleteAllAction = new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), tr("Delete &All..."), m_geographicDataMenu);
	QAction* exportAllPolygonsAction = new QAction(QIcon(":/libs/guibase/images/iconExport.png"), tr("Export All Polygons..."), m_geographicDataMenu);

	m_geographicDataMenu->addAction(editGroupAction);
	m_geographicDataMenu->addSeparator();

	if (item != 0){
		// Rawdata dataitem is selected.
		QList<RawDataCreator*> creators = RawDataFactory::instance().compatibleCreators(dynamic_cast<PreProcessorRawDataGroupDataItem*>(item->parent())->condition());
		for (QList<RawDataCreator*>::iterator it = creators.begin(); it != creators.end(); ++it){
			if (dynamic_cast<RawDataPolygonCreator*>(*it) != 0){
				polygonCreator = *it;
			}
		}
		setupRawDataAddActions(dynamic_cast<PreProcessorRawDataGroupDataItem*>(item->parent()));
		QMenu* dummy;

		RawData* raw = item->rawData();
		RawDataRiverSurvey* s = dynamic_cast<RawDataRiverSurvey*>(raw);
		dummy = new QMenu(tr("&River Survey"), m_geographicDataMenu);
		if (s == 0){
			dummy->setDisabled(true);
		}else{
			dummy->addActions(s->menu()->actions());
		}
		m_geographicDataMenu->addMenu(dummy);

		RawDataPointmap* pm = dynamic_cast<RawDataPointmap*>(raw);
		dummy = new QMenu(tr("P&ointset Data"), m_geographicDataMenu);
		if (pm == 0){
			dummy->setDisabled(true);
		} else {
			dummy->addActions(pm->menu()->actions());
		}
		m_geographicDataMenu->addMenu(dummy);

		RawDataPolygon* pol = dynamic_cast<RawDataPolygon*>(raw);
		dummy = new QMenu(tr("&Polygon"), m_geographicDataMenu);
		dummy->addAction(m_rawDataAddActions.value(polygonCreator));
		if (pol != 0){
			dummy->addSeparator();
			dummy->addActions(pol->menu()->actions());
		}
		m_geographicDataMenu->addMenu(dummy);
		deleteAllAction->setDisabled(true);
		exportAllPolygonsAction->setDisabled(true);
	} else {
		PreProcessorRawDataGroupDataItem* gitem = dynamic_cast<PreProcessorRawDataGroupDataItem*>(m_selectedItem);
		if (gitem != 0){
			// Rawdatagroup dataitem is selected.
			QList<RawDataCreator*> creators = RawDataFactory::instance().compatibleCreators(gitem->condition());
			for (QList<RawDataCreator*>::iterator it = creators.begin(); it != creators.end(); ++it){
				if (dynamic_cast<RawDataPolygonCreator*>(*it) != 0){
					polygonCreator = *it;
				}
			}
			setupRawDataAddActions(gitem);
			QMenu* dummy;
			dummy = new QMenu(tr("&River Survey"), m_geographicDataMenu);
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			dummy = new QMenu(tr("P&ointset Data"), m_geographicDataMenu);
			dummy->setDisabled(true);
			m_geographicDataMenu->addMenu(dummy);
			dummy = new QMenu(tr("&Polygon"), m_geographicDataMenu);
			dummy->addAction(m_rawDataAddActions.value(polygonCreator));
			m_geographicDataMenu->addMenu(dummy);

			PreProcessorRawDataComplexGroupDataItem* cgitem = dynamic_cast<PreProcessorRawDataComplexGroupDataItem*>(gitem);
			if (cgitem != 0){
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
	if (gti != 0){
		QList<PreProcessorRawDataGroupDataItemInterface*> groups = gti->rawdataTop()->groupDataItems();
		for (QList<PreProcessorRawDataGroupDataItemInterface*>::iterator it = groups.begin(); it != groups.end(); ++it){
			PreProcessorRawDataGroupDataItemInterface* groupDataitem = *it;
			QString condCaption = groupDataitem->condition()->caption();
			condCaption.append("...");
			QAction* action = new QAction(condCaption, colorMapMenu);
			connect(action, SIGNAL(triggered()), groupDataitem, SLOT(editScalarsToColors()));
			colorMapMenu->addAction(action);
		}
		connect(setupScalarBarAction, SIGNAL(triggered()), gti->rawdataTop(), SLOT(setupScalarBar()));
	}else{
		colorMapMenu->setDisabled(true);
		setupScalarBarAction->setDisabled(true);
	}
	m_geographicDataMenu->addSeparator();

	PreProcessorWindow* preWindow = dynamic_cast<PreProcessorWindow*>(mainWindow());
	QMenu* importMenu = new QMenu(tr("Import"), m_geographicDataMenu);
	importMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	m_geographicDataMenu->addMenu(importMenu);
	connect(importMenu, SIGNAL(aboutToShow()), preWindow, SLOT(setupRawDataImportMenu()));
	QMenu* exportMenu = new QMenu(tr("Export"), m_geographicDataMenu);
	exportMenu->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	m_geographicDataMenu->addMenu(exportMenu);
	connect(exportMenu, SIGNAL(aboutToShow()), preWindow, SLOT(setupRawDataExportMenu()));

	m_geographicDataMenu->addAction(exportAllPolygonsAction);
}

QMenu* PreProcessorDataModel::setupGridCreationMenu(QMenu* parentMenu, PreProcessorGridCreatingConditionDataItemInterface* /*gcItem*/)
{
	QMenu* gridCreationMenu = new QMenu(tr("&Grid Creating Condition"), parentMenu);
	connect(gridCreationMenu, SIGNAL(aboutToShow()), this, SLOT(setupGridCreationMenuContent()));
	return gridCreationMenu;
}

QMenu* PreProcessorDataModel::setupBoundaryConditionSettingMenu(QMenu* parentMenu){
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
	if (gItem != 0){
		// ----------------------------
		// grid creating related menus.
		// ----------------------------

		PreProcessorGridCreatingConditionDataItemInterface* gcItem = 0;
		gcItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(gItem->parent())->creatingConditionDataItem();
		m_gridMenu->addAction(gcItem->switchAction());
		// grid creating condition menu
		QMenu* creationMenu = setupGridCreationMenu(m_gridMenu, gcItem);
		m_gridMenu->addMenu(creationMenu);
		if (gItem->bcGroupDataItem() != 0){
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
		PreProcessorGridRelatedConditionNodeDataItem* nItem =
				dynamic_cast<PreProcessorGridRelatedConditionNodeDataItem*>(m_selectedItem);
		gItem->setNodeDataItem(nItem);
		if (nItem != 0){
			gItem->nodeEditAction()->setEnabled(gItem->selectedVertices().count() > 0);
			gItem->nodeEditAction()->disconnect();
			connect(gItem->nodeEditAction(), SIGNAL(triggered()), nItem, SLOT(editValue()));
			gItem->nodeDisplaySettingAction()->setEnabled(true);
			gItem->nodeDisplaySettingAction()->disconnect();
			connect(gItem->nodeDisplaySettingAction(), SIGNAL(triggered()), nItem, SLOT(showDisplaySettingDialog()));
		}else{
			gItem->nodeEditAction()->setEnabled(false);
			gItem->nodeEditAction()->disconnect();
			gItem->nodeDisplaySettingAction()->setEnabled(false);
		}
		// disable cell related menus when cell dataitem is not selected currently.
		PreProcessorGridRelatedConditionCellDataItem* cItem =
				dynamic_cast<PreProcessorGridRelatedConditionCellDataItem*>(m_selectedItem);
		gItem->setCellDataItem(cItem);
		if (cItem != 0){
			gItem->cellEditAction()->setEnabled(gItem->selectedCells().count() > 0);
			gItem->cellEditAction()->disconnect();
			connect(gItem->cellEditAction(), SIGNAL(triggered()), cItem, SLOT(editValue()));
			gItem->cellDisplaySettingAction()->setEnabled(true);
			gItem->cellDisplaySettingAction()->disconnect();
			connect(gItem->cellDisplaySettingAction(), SIGNAL(triggered()), cItem, SLOT(showDisplaySettingDialog()));
		}else{
			gItem->cellEditAction()->setEnabled(false);
			gItem->cellEditAction()->disconnect();
			gItem->cellDisplaySettingAction()->setEnabled(false);
		}

		PreProcessorBCDataItem* bcItem =
			dynamic_cast<PreProcessorBCDataItem*>(m_selectedItem);
		if (gItem->bcGroupDataItem() != 0){
			gItem->bcGroupDataItem()->bcMenu()->setDisabled(gItem->grid() == 0);
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
	}else{
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

	if (fitem == 0){
		pointAction->setDisabled(true);
		vectorAction->setDisabled(true);
	} else{
		connect(pointAction, SIGNAL(triggered()), fitem->pointGroupDataItem(), SLOT(showSettingDialog()));
		connect(vectorAction, SIGNAL(triggered()), fitem->vectorGroupDataItem(), SLOT(showSettingDialog()));
	}
	m_additionalMenus.append(m_measuredValuesMenu);
	m_dummyMenus.append(m_measuredValuesMenu);
}

PreProcessorGridTypeDataItem* PreProcessorDataModel::getGridTypeItem(GraphicsWindowDataItem* item)
{
	if (item == 0){return 0;}
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(item);
	if (r != 0){return 0;}
	PreProcessorGridTypeDataItem* g = dynamic_cast<PreProcessorGridTypeDataItem*>(item);
	if (g != 0){return g;}
	return getGridTypeItem(dynamic_cast<GraphicsWindowDataItem*>(item->parent()));
}

PreProcessorGridDataItem* PreProcessorDataModel::getGridItem(GraphicsWindowDataItem* item)
{
	PreProcessorGridDataItem* gItem = getGridItemRecursively(item);
	if (gItem != 0){return gItem;}
	// try to find the gridtype node that contains the current item.
	PreProcessorGridTypeDataItem* gtItem = getGridTypeItem(item);
	if (gtItem != 0){
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gtItem->conditions();
		if (conds.count() > 0){
			// select the first one.
			PreProcessorGridAndGridCreatingConditionDataItemInterface* gagcItem = conds[0];
			return dynamic_cast<PreProcessorGridDataItem*> (gagcItem->gridDataItem());
		}
	}
	// try to find some grid dataitem in one of the gridtypes.
	PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gtItems = rItem->gridTypeDataItems();
	QList<PreProcessorGridTypeDataItem*>::iterator gtIt;
	for (gtIt = gtItems.begin(); gtIt != gtItems.end(); ++gtIt){
		PreProcessorGridTypeDataItem* gtItem = *gtIt;
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gtItem->conditions();
		if (conds.count() > 0){
			// select the first one.
			PreProcessorGridAndGridCreatingConditionDataItemInterface* gagcItem = conds[0];
			return dynamic_cast<PreProcessorGridDataItem*> (gagcItem->gridDataItem());
		}
	}
	return 0;
}

PreProcessorGridDataItem* PreProcessorDataModel::getGridItemRecursively(GraphicsWindowDataItem* item)
{
	if (item == 0){return 0;}
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(item);
	if (r != 0){
		// reached root data item.
		return 0;
	}
	PreProcessorGridDataItem* g = dynamic_cast<PreProcessorGridDataItem*>(item);
	if (g != 0){return g;}
	PreProcessorGridAndGridCreatingConditionDataItem* a = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(item);
	if (a != 0){
		return dynamic_cast<PreProcessorGridDataItem*> (a->gridDataItem());
	}
	return getGridItemRecursively(dynamic_cast<GraphicsWindowDataItem*>(item->parent()));
}

MeasuredDataFileDataItem* PreProcessorDataModel::getMeasuredDataItem(GraphicsWindowDataItem* item)
{
	MeasuredDataFileDataItem* fItem = getMeasuredDataItemRecursively(item);
	if (fItem != 0){return fItem;}
	// if there is only one Measured data, use that.
	PreProcessorRootDataItem* rItem = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	PreProcessorMeasuredDataTopDataItem* mtItem = rItem->measuredDataTopDataItem();
	if (mtItem->childItems().count() == 1){
		MeasuredDataFileDataItem* ret = dynamic_cast<MeasuredDataFileDataItem*>(mtItem->childItems().at(0));
		return ret;
	}
	return 0;
}

MeasuredDataFileDataItem* PreProcessorDataModel::getMeasuredDataItemRecursively(GraphicsWindowDataItem* item)
{
	if (item == 0){return 0;}
	PreProcessorRootDataItem* r = dynamic_cast<PreProcessorRootDataItem*>(item);
	if (r != 0){
		// reached root data item.
		return 0;
	}
	MeasuredDataFileDataItem* f = dynamic_cast<MeasuredDataFileDataItem*>(item);
	if (f != 0){return f;}
	return getMeasuredDataItemRecursively(dynamic_cast<GraphicsWindowDataItem*>(item->parent()));
}

PreProcessorBackgroundImageDataItem* PreProcessorDataModel::getBackgroundImageItem(GraphicsWindowDataItem* item)
{
	if (item == 0){return 0;}
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
	if (gridTypes.count() == 0){
		// no menu available.
	}else if (gridTypes.count() == 1){
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		importAvailable = addGridImportMenuForGridType(menu, gt, true);
	}else{
		for (int i = 0; i < gridTypes.count(); ++i){
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (addGridImportMenuForGridType(gtMenu, gt, false)){
				importAvailable = true;
			}else{
				delete gtMenu;
			}
		}
	}
	if (! importAvailable){
		QAction* no = menu->addAction(tr("&Grid..."));
		no->setDisabled(true);
	}
}

bool PreProcessorDataModel::addGridImportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone)
{
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gt->conditions();
	if (conds.count() == 0){
		// no menu available.
		return false;
	}else if (conds.count() == 1){
		PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(0);
		if (alone){
			// this is the only existing grid.
			if (! addGridImportMenuForGrid(menu, di, tr("&Grid"))){
				return false;
			}
		}else{
			if (! addGridImportMenuForGrid(menu, di, di->caption())){
				return false;
			}
		}
		return true;
	}else{
		// there are multiple grids.
		bool okExists = false;
		QMenu* tMenu = menu->addMenu(tr("&Grid"));
		for (int i = 0; i < conds.count(); ++i){
			PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(i);
			QMenu* gMenu = tMenu->addMenu(di->caption());
			if (addGridImportMenuForGrid(gMenu, di, di->caption())){
				okExists = true;
			}else{
				delete gMenu;
			}
		}
		if (! okExists){
			delete tMenu;
		}
		return okExists;
	}
}

bool PreProcessorDataModel::addGridImportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name)
{
	QString cap = QString("%1...").arg(name);
	PreProcessorGridDataItem* gdi = dynamic_cast<PreProcessorGridDataItem*> (di->gridDataItem());
	if (! gdi->isImportAvailable()){return false;}
	QAction* action = new QAction(cap, menu);
	connect(action, SIGNAL(triggered()), gdi, SLOT(importGrid()));
	menu->addAction(action);
	return true;
}


void PreProcessorDataModel::addGridExportMenu(QMenu* menu)
{
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool exportAvailable = false;
	if (gridTypes.count() == 0){
		// no menu available.
	}else if (gridTypes.count() == 1){
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		exportAvailable = addGridExportMenuForGridType(menu, gt, true);
	}else{
		for (int i = 0; i < gridTypes.count(); ++i){
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (addGridExportMenuForGridType(gtMenu, gt, false)){
				exportAvailable = true;
			}else{
				delete gtMenu;
			}
		}
	}
	if (! exportAvailable){
		QAction* no = menu->addAction(tr("&Grid..."));
		no->setDisabled(true);
	}
}

bool PreProcessorDataModel::addGridExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt, bool alone)
{
	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*> conds = gt->conditions();
	if (conds.count() == 0){
		// no menu available.
		return false;
	}else if (conds.count() == 1){
		PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(0);
		if (alone){
			// this is the only existing grid.
			if (! addGridExportMenuForGrid(menu, di, tr("&Grid"))){
				return false;
			}
		}else{
			if (! addGridExportMenuForGrid(menu, di, di->caption())){
				return false;
			}
		}
		return true;
	}else{
		// there are multiple grids.
		bool okExists = false;
		QMenu* tMenu = menu->addMenu(tr("&Grid"));
		for (int i = 0; i < conds.count(); ++i){
			PreProcessorGridAndGridCreatingConditionDataItemInterface* di = conds.at(i);
			QMenu* gMenu = tMenu->addMenu(di->caption());
			if (addGridExportMenuForGrid(gMenu, di, di->caption())){
				okExists = true;
			}else{
				delete gMenu;
			}
		}
		if (! okExists){
			delete tMenu;
		}
		return okExists;
	}
}

bool PreProcessorDataModel::addGridExportMenuForGrid(QMenu* menu, PreProcessorGridAndGridCreatingConditionDataItemInterface* di, const QString& name)
{
	QString cap = QString("%1...").arg(name);
	PreProcessorGridDataItem* gdi = dynamic_cast<PreProcessorGridDataItem*> (di->gridDataItem());
	if (! gdi->isExportAvailable()){return false;}
	QAction* action = new QAction(cap, menu);
	menu->addAction(action);
	connect(action, SIGNAL(triggered()), gdi, SLOT(exportGrid()));
	return true;
}

void PreProcessorDataModel::setupRawDataImportMenu(QMenu *menu)
{
	// clear menu first.
	menu->clear();
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool importAvailable = false;
	if (gridTypes.count() == 0){
		// no menu available.
	}else if (gridTypes.count() == 1){
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		importAvailable = setupRawDataImportMenuForGridType(menu, gt);
	}else{
		for (int i = 0; i < gridTypes.count(); ++i){
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (setupRawDataImportMenuForGridType(gtMenu, gt)){
				importAvailable = true;
			}else{
				delete gtMenu;
			}
		}
	}
	if (! importAvailable){
		QAction* no = menu->addAction(tr("(No data to import)"));
		no->setDisabled(true);
	}
}


void PreProcessorDataModel::setupHydraulicDataImportMenu(QMenu *menu)
{
	// clear menu first.
	menu->clear();

	QList<HydraulicDataImporter*> importers = HydraulicDataImporterFactory::instance().importers();
	for (QList<HydraulicDataImporter*>::iterator it = importers.begin(); it != importers.end(); ++it){
		HydraulicDataImporter* imp = *it;
		QAction* action = menu->addAction(imp->caption());
		connect(action, SIGNAL(triggered()), this, SLOT(importHydraulicData()));
	}
}

bool PreProcessorDataModel::setupRawDataImportMenuForGridType(QMenu *menu, PreProcessorGridTypeDataItem *gt)
{
	PreProcessorRawDataTopDataItemInterface* topItem = gt->rawdataTop();
	QList<PreProcessorRawDataGroupDataItemInterface*> groups = topItem->groupDataItems();
	if (groups.count() == 0){
		// no menu available.
		return false;
	}else{
		// there are multiple grids.
		bool okExists = false;
		for (int i = 0; i < groups.count(); ++i){
			PreProcessorRawDataGroupDataItemInterface* gi = groups.at(i);
			setupRawDataImportMenuForGroup(menu, gi);
			okExists = true;
		}
		return okExists;
	}
}

bool PreProcessorDataModel::setupRawDataImportMenuForGroup(QMenu *menu, PreProcessorRawDataGroupDataItemInterface *gt)
{
	PreProcessorRawDataGroupDataItem* item = dynamic_cast<PreProcessorRawDataGroupDataItem*> (gt);
	item->addImportAction(menu);
	return true;
}

void PreProcessorDataModel::setupRawDataExportMenu(QMenu *menu)
{
	// clear menu first.
	menu->clear();
	// find how many grid types are available.
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*>(m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> gridTypes = root->gridTypeDataItems();
	bool importAvailable = false;
	if (gridTypes.count() == 0){
		// no menu available.
	}else if (gridTypes.count() == 1){
		PreProcessorGridTypeDataItem* gt = gridTypes.at(0);
		importAvailable = setupRawDataExportMenuForGridType(menu, gt);
	}else{
		for (int i = 0; i < gridTypes.count(); ++i){
			PreProcessorGridTypeDataItem* gt = gridTypes.at(i);
			QMenu* gtMenu = menu->addMenu(gt->gridType()->caption());
			if (setupRawDataExportMenuForGridType(gtMenu, gt)){
				importAvailable = true;
			}else{
				delete gtMenu;
			}
		}
	}
	if (! importAvailable){
		QAction* no = menu->addAction(tr("(No data to export)"));
		no->setDisabled(true);
	}
}

bool PreProcessorDataModel::setupRawDataExportMenuForGridType(QMenu* menu, PreProcessorGridTypeDataItem* gt)
{
	PreProcessorRawDataTopDataItemInterface* topItem = gt->rawdataTop();
	QList<PreProcessorRawDataGroupDataItemInterface*> groups = topItem->groupDataItems();
	if (groups.count() == 0){
		// no menu available.
		return false;
	}else{
		for (int i = 0; i < groups.count(); ++i){
			PreProcessorRawDataGroupDataItemInterface* gi = groups.at(i);
			QString cap = gi->condition()->caption();
			cap.append("...");
			QAction* exportAction = menu->addAction(cap);
			exportAction->setEnabled(rawDataExportAvailable(gi));
			connect(exportAction, SIGNAL(triggered()), gi, SLOT(doExport()));
		}
	}
	return true;
}

bool PreProcessorDataModel::setupRawDataExportMenuForGroup(QMenu* menu, PreProcessorRawDataGroupDataItem* gt)
{
	bool ok = false;
	QList<PreProcessorRawdataDataItemInterface*> datas = gt->rawDatas();
	for (int i = 0; i < datas.count(); ++i){
		PreProcessorRawdataDataItemInterface* item = datas.at(i);
		QMenu* tmpmenu = menu->addMenu(item->standardItem()->text());
		if (setupRawDataExportMenuForItem(tmpmenu, item)){
			ok = true;
		}else{
			delete tmpmenu;
		}
	}
	return ok;
}

bool PreProcessorDataModel::rawDataExportAvailable(PreProcessorRawDataGroupDataItemInterface* gt)
{
	QList<PreProcessorRawdataDataItemInterface*> datas = gt->rawDatas();
	for (int i = 0; i < datas.count(); ++i){
		PreProcessorRawdataDataItemInterface* item = datas.at(i);
		if (item->rawData()->exporters().count() > 0){
			return true;
		}
	}
	return false;
}

bool PreProcessorDataModel::setupRawDataExportMenuForItem(QMenu* menu, PreProcessorRawdataDataItemInterface* gt)
{
	return dynamic_cast<PreProcessorRawdataDataItem*>(gt)->setupExportMenu(menu);
}

void PreProcessorDataModel::setupGridCreationMenuContent()
{
	QMenu* gridCreationMenu = dynamic_cast<QMenu*>(sender());
	gridCreationMenu->clear();
	// Add Grid Creation menu.
	PreProcessorGridDataItem* gItem = getGridItem(m_selectedItem);
	if (gItem == 0){return;}
	PreProcessorGridCreatingConditionDataItemInterface* gcItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(gItem->parent())->creatingConditionDataItem();
	if (gcItem == 0){return;}
	QMenu* dummy = dynamic_cast<PreProcessorGridCreatingConditionDataItem*> (gcItem)->menu();
	if (dummy == 0){
		QAction* dummyAction = gridCreationMenu->addAction(tr("(No algorithm is selected yet)"));
		dummyAction->setDisabled(true);
	}else{
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
	if (gItem == 0){return;}
	PreProcessorBCSettingGroupDataItem* bcsgItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(gItem->parent())
			->bcSettingGroupDataItem();
	PreProcessorBCSettingDataItem* bcItem = getBCSettingDataItem(m_selectedItem);
	if (bcItem != 0){
		bcsMenu->addAction(bcItem->editAction());
	} else {
		bcsMenu->addAction(bcsgItem->dummyEditAction());
	}
	bcsMenu->addSeparator();
	const QList<QAction*>& addActions = bcsgItem->addActions();
	QList<QAction*>::const_iterator it;
	for (it = addActions.begin(); it != addActions.end(); ++it){
		bcsMenu->addAction(*it);
	}
	bcsMenu->addSeparator();
	if (bcItem != 0){
		bcsMenu->addAction(bcItem->deleteAction());
	} else {
		bcsMenu->addAction(bcsgItem->dummyDeleteAction());
	}
}

void PreProcessorDataModel::disableGridMenu()
{
	m_gridMenu->setDisabled(true);
}

void PreProcessorDataModel::setupRawDataAddActions(PreProcessorRawDataGroupDataItem* item)
{
	if (m_rawDataAddSignalMapper != 0){delete m_rawDataAddSignalMapper;}
	m_rawDataAddSignalMapper = new QSignalMapper(this);
	QMap<RawDataCreator*, QAction*>::iterator it;
	for (it = m_rawDataAddActions.begin(); it != m_rawDataAddActions.end(); ++it){
		delete *it;
	}
	m_rawDataAddActions.clear();
	QList<RawDataCreator*> creators = RawDataFactory::instance().compatibleCreators(item->condition());
	QList<RawDataCreator*>::iterator cit;
	for (cit = creators.begin(); cit != creators.end(); ++cit){
		QAction* action = new QAction(tr("Add New %1").arg((*cit)->caption()), this);
		m_rawDataAddActions.insert(*cit, action);
		m_rawDataAddSignalMapper->setMapping(action, *cit);
		connect(action, SIGNAL(triggered()), m_rawDataAddSignalMapper, SLOT(map()));
	}
	connect(m_rawDataAddSignalMapper, SIGNAL(mapped(QObject*)), item, SLOT(addRawData(QObject*)));
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
	QList<PreProcessorGridTypeDataItem*>::iterator it;
	for (it = gtItems.begin(); it != gtItems.end(); ++it){
		PreProcessorGridTypeDataItem* gtItem = *it;
		QList<PreProcessorRawDataGroupDataItemInterface*> gitems = gtItem->rawdataTop()->groupDataItems();
		QList<PreProcessorRawDataGroupDataItemInterface*>::iterator it2;
		for (it2 = gitems.begin(); it2 != gitems.end(); ++it2){
			PreProcessorRawDataGroupDataItemInterface* gItem = *it2;
			QList<PreProcessorRawdataDataItemInterface*> rawdatas = gItem->rawDatas();
			QList<PreProcessorRawdataDataItemInterface*>::iterator it3;
			for (it3 = rawdatas.begin(); it3 != rawdatas.end(); ++it3){
				PreProcessorRawdataDataItemInterface* dItem = *it3;
				RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(dItem->rawData());
				if (rs != 0){
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
	QList<PreProcessorGridTypeDataItem*>::iterator it;
	bool mapExexuted = false;
	for (it = gtItems.begin(); it != gtItems.end(); ++it){
		QStringList notMapped;
		PreProcessorGridTypeDataItem* gtItem = *it;
		QList<PreProcessorRawDataGroupDataItemInterface*> gitems = gtItem->rawdataTop()->groupDataItems();
		QList<PreProcessorRawDataGroupDataItemInterface*>::iterator it2;
		QList<PreProcessorRawDataGroupDataItemInterface*> groupsToMap;
		for (it2 = gitems.begin(); it2 != gitems.end(); ++it2){
			PreProcessorRawDataGroupDataItemInterface* gItem = *it2;
			QStringList rawdatasNotMapped = dynamic_cast<PreProcessorRawDataGroupDataItem*> (gItem)->getRawDatasNotMapped();
			if (rawdatasNotMapped.count() > 0){
				groupsToMap.append(gItem);
				notMapped.append(rawdatasNotMapped);
			}
		}
		if (notMapped.count() > 0){
			// rawdata in this grid type is not mapped.
			int ret = QMessageBox::warning(m_mainWindow, PreProcessorDataModel::tr("Warning"), PreProcessorDataModel::tr("%1 are not mapped after they are edited last time. Do you want to execute mapping now?").arg(notMapped.join(", ")), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
			if (ret == QMessageBox::Yes){
				// execute mapping for each grids.
				const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions = gtItem->conditions();
				QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::const_iterator cit;
				for (cit = conditions.begin(); cit != conditions.end(); ++cit){
					PreProcessorGridAndGridCreatingConditionDataItemInterface* cond = *cit;
					PreProcessorGridAndGridCreatingConditionDataItem* cond2 = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (cond);
					PreProcessorGridAttributeMappingSettingTopDataItem* tItem = cond2->mappingSettingDataItem();
					for (int i = 0; i < groupsToMap.count(); ++i){
						PreProcessorRawDataGroupDataItemInterface* gItem = groupsToMap[i];
						tItem->customMapping(gItem->condition()->name(), true);
					}
				}
				mapExexuted = true;
			} else if (ret == QMessageBox::Cancel){
				return false;
			}
		}

		const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions = gtItem->conditions();
		QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::const_iterator cit;
		for (cit = conditions.begin(); cit != conditions.end(); ++cit){
			PreProcessorGridAndGridCreatingConditionDataItem* cond = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (*cit);
			const QList <GraphicsWindowDataItem*>& bcslist = cond->bcSettingGroupDataItem()->childItems();
			QList <GraphicsWindowDataItem*>::const_iterator cit;
			for (cit = bcslist.begin(); cit != bcslist.end(); ++cit){
				PreProcessorBCSettingDataItem* bcsdi = dynamic_cast<PreProcessorBCSettingDataItem*>(*cit);
				if (! bcsdi->isMapped()){
					int ret = QMessageBox::warning(m_mainWindow, PreProcessorDataModel::tr("Warning"), PreProcessorDataModel::tr("Boundary Condition Setting \"%1\" is not mapped after it is edited last time. Do you want to execute mapping now?").arg(bcsdi->bcDataItem()->caption()), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::No);
					if (ret == QMessageBox::Yes){
						bcsdi->executeMapping(true, 0);
						mapExexuted = true;
					} else if (ret == QMessageBox::Cancel){
						return false;
					}
				}
			}
		}
	}


	if (mapExexuted){
		QMessageBox::information(mainWindow(), tr("Information"), tr("Mapping geographic data, boundary condition finished successfully."));
	}
	return ! mapExexuted;
}

void PreProcessorDataModel::importHydraulicData()
{
	QAction* action = dynamic_cast<QAction*> (sender());

	QList<HydraulicDataImporter*> importers = HydraulicDataImporterFactory::instance().importers();
	HydraulicDataImporter* importer = 0;
	for (QList<HydraulicDataImporter*>::iterator it = importers.begin(); it != importers.end(); ++it){
		HydraulicDataImporter* imp = *it;
		if (imp->caption() == action->text()){
			importer = imp;
			break;
		}
	}
	if (importer == 0){return;}
	// scan all rawdata.
	QList<RawData*> rawdatas;

	PreProcessorRootDataItem* ritem = dynamic_cast<PreProcessorRootDataItem*> (m_rootDataItem);
	QList<PreProcessorGridTypeDataItem*> titems = ritem->gridTypeDataItems();
	QList<PreProcessorGridTypeDataItem*>::iterator tit;
	for (tit = titems.begin(); tit != titems.end(); ++tit) {
		PreProcessorGridTypeDataItem* titem = *tit;
		QList<PreProcessorRawDataGroupDataItemInterface*> gitems = titem->rawdataTop()->groupDataItems();
		QList<PreProcessorRawDataGroupDataItemInterface*>::iterator git;
		for (git = gitems.begin(); git != gitems.end(); ++git) {
			PreProcessorRawDataGroupDataItemInterface* gitem = *git;
			QList<PreProcessorRawdataDataItemInterface*> ditems = gitem->rawDatas();
			QList<PreProcessorRawdataDataItemInterface*>::iterator dit;
			for (dit = ditems.begin(); dit != ditems.end(); ++dit) {
				PreProcessorRawdataDataItemInterface* ditem = *dit;
				RawData* rdata = ditem->rawData();
				if (importer->canImportTo(rdata)) {
					// this importer can used for this raw data.
					rawdatas.append(rdata);
				}
			}
		}
	}
	RawData* targetRawdata = 0;
	if (rawdatas.count() == 0) {
		QMessageBox::warning(mainWindow(), tr("Warning"), tr("There is no geographic data to import this hydraulic data."));
		return;
	} else if (rawdatas.count() > 1) {
		ItemSelectingDialog dialog(mainWindow());
		QList<QString> items;
		for (int i = 0; i < rawdatas.count(); ++i) {
			items.append(rawdatas.at(i)->caption());
		}
		dialog.setItems(items);
		dialog.setWindowTitle(tr("Select geographic data"));
		dialog.setMessage(tr("Please select the geographic data to import hydraulic data."));
		int ret = dialog.exec();
		if (ret == QDialog::Rejected){return;}
		targetRawdata = rawdatas.at(dialog.selectIndex());
	} else {
		// there was only one rawdata.
		targetRawdata = rawdatas.at(0);
	}
	// get filename.
	QString dir = LastIODirectory::get();
	QString selectedFilter;
	// Select the file to import.
	QStringList filters = importer->fileDialogFilters();
	QString filename = QFileDialog::getOpenFileName(mainWindow(), tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()){return;}
	bool result = importer->import(targetRawdata, filename, selectedFilter, mainWindow());
	if (result) {
		QMessageBox::information(mainWindow(), tr("Information"), tr("Importing %1 succeeded.").arg(QDir::toNativeSeparators(filename)));
		QFileInfo finfo(filename);
		LastIODirectory::set(finfo.absolutePath());
	} else {
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Importing %1 failed.").arg(QDir::toNativeSeparators(filename)));
	}
}

PreProcessorRawDataTopDataItemInterface* PreProcessorDataModel::rawDataTopDataItem(const QString& type) const
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*> (m_rootDataItem);
	PreProcessorGridTypeDataItem* tItem = root->gridTypeDataItem(type);
	if (tItem == 0){return 0;}
	return tItem->rawdataTop();
}

PreProcessorGridAndGridCreatingConditionDataItemInterface* PreProcessorDataModel::getGridAndGridCreatingConditionDataItem(const QString& typeName, const QString& zoneName) const
{
	PreProcessorRootDataItem* root = dynamic_cast<PreProcessorRootDataItem*> (m_rootDataItem);
	return root->gridTypeDataItem(typeName)->condition(zoneName);
}

void PreProcessorDataModel::applyOffset(double x, double y)
{
	m_rootDataItem->applyOffset(x, y);
}
