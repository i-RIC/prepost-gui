#include "../factory/gridexporterfactory.h"
#include "../factory/gridimporterfactory.h"
#include "../gridexporter/cgnsgridexporter.h"
#include "../gridimporter/cgnsgridimporter.h"
#include "../gridimporter/projectgridimporter.h"
#include "../misc/gridattributegeneratorlauncher.h"
#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "../subwindow/gridbirdeyewindow/gridbirdeyewindow.h"
#include "../subwindow/gridbirdeyewindow/gridbirdeyewindowprojectdataitem.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributeiedgegroupdataitem.h"
#include "preprocessorgridattributejedgegroupdataitem.h"
#include "preprocessorgridattributemappingsettingtopdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "private/preprocessorgriddataitem_cgnsexporter.h"
#include "private/preprocessorgriddataitem_cgnsimporter.h"
#include "private/preprocessorgriddataitem_impl.h"
#include "private/preprocessorgriddataitem_projectimporter.h"
#include "public/preprocessorgriddataitem_selectedcellscontroller.h"
#include "public/preprocessorgriddataitem_selectededgescontroller.h"
#include "public/preprocessorgriddataitem_selectediedgescontroller.h"
#include "public/preprocessorgriddataitem_selectedjedgescontroller.h"
#include "public/preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4gridstructurecheckeri.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/grid/v4inputgridio.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

PreProcessorGridDataItem::Impl::Impl(PreProcessorGridDataItem* item) :
	m_grid {nullptr},
	m_nodeDataItem {nullptr},
	m_cellDataItem {nullptr},
	m_birdEyeWindow {nullptr},
	m_selectedNodesController {new SelectedNodesController {item}},
	m_selectedCellsController {new SelectedCellsController {item}},
	m_selectedIEdgesController {new SelectedIEdgesController {item}},
	m_selectedJEdgesController {new SelectedJEdgesController {item}},
	m_selectedEdgesController {new SelectedEdgesController {item}},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_menu {nullptr},
	m_generateAttMenu {nullptr},
	m_shiftPressed {false},
	m_gridIsDeleted {false},
	m_item {item}
{}

PreProcessorGridDataItem::Impl::~Impl()
{
	delete m_selectedNodesController;
	delete m_selectedCellsController;
	delete m_selectedIEdgesController;
	delete m_selectedJEdgesController;
	delete m_selectedEdgesController;
	delete m_grid;
	delete m_menu;
	delete m_generateAttMenu;
}

PreProcessorGridCrosssectionWindow* PreProcessorGridDataItem::Impl::buildCrosssectionWindow()
{
	auto window = new PreProcessorGridCrosssectionWindow(m_item, m_item->mainWindow());

	auto center = dynamic_cast<QMdiArea*> (m_item->iricMainWindow()->centralWidget());
	auto container = center->addSubWindow(window);
	container->setWindowIcon(window->windowIcon());

	return window;
}

PreProcessorGridDataItem::PreProcessorGridDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridDataItemI(parent),
	m_bcGroupDataItem {nullptr},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_nodeGroupDataItem = new PreProcessorGridAttributeNodeGroupDataItem(this);
	m_childItems.push_back(m_nodeGroupDataItem);

	m_cellGroupDataItem = new PreProcessorGridAttributeCellGroupDataItem(this);
	m_cellGroupDataItem->setupChildren();
	m_childItems.push_back(m_cellGroupDataItem);

	m_iEdgeGroupDataItem = new PreProcessorGridAttributeIEdgeGroupDataItem(this);
	m_iEdgeGroupDataItem->setupChildren();
	m_childItems.push_back(m_iEdgeGroupDataItem);

	m_jEdgeGroupDataItem = new PreProcessorGridAttributeJEdgeGroupDataItem(this);
	m_jEdgeGroupDataItem->setupChildren();
	m_childItems.push_back(m_jEdgeGroupDataItem);

	setupActors();
	setupActions();
}

PreProcessorGridDataItem::~PreProcessorGridDataItem()
{
	renderer()->RemoveActor(impl->m_regionActor);
	closeBirdEyeWindow();
	closeCrosssectionWindows();

	delete m_bcGroupDataItem;
	delete impl;
}

void PreProcessorGridDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()) {m_shapeDataItem->loadFromProjectMainFile(shapeNode);}

	QDomNode nodeNode = iRIC::getChildNode(node, "NodeAttributes");
	if (! nodeNode.isNull()) {m_nodeGroupDataItem->loadFromProjectMainFile(nodeNode);}

	QDomNode cellNode = iRIC::getChildNode(node, "CellAttributes");
	if (! cellNode.isNull()) {m_cellGroupDataItem->loadFromProjectMainFile(cellNode);}

	QDomNode iEdgeNode = iRIC::getChildNode(node, "IEdgeAttributes");
	if (! iEdgeNode.isNull()) {m_iEdgeGroupDataItem->loadFromProjectMainFile(iEdgeNode);}

	QDomNode jEdgeNode = iRIC::getChildNode(node, "JEdgeAttributes");
	if (! jEdgeNode.isNull()) {m_jEdgeGroupDataItem->loadFromProjectMainFile(jEdgeNode);}

	QDomNode bcNode = iRIC::getChildNode(node, "BoundaryConditions");
	if (! bcNode.isNull() && m_bcGroupDataItem != nullptr) {
		m_bcGroupDataItem->loadFromProjectMainFile(bcNode);
	}

	QDomNode beNode = iRIC::getChildNode(node, "GridBirdEyeWindow");
	if (! beNode.isNull()) {
		openBirdEyeWindow();
		if (impl->m_birdEyeWindow != nullptr) {
			impl->m_birdEyeWindow->loadFromProjectMainFile(beNode);
		}
	}
	QDomNode cwNode = iRIC::getChildNode(node, "CrossSectionWindows");
	if (! cwNode.isNull()) {
		const auto& clist = cwNode.childNodes();
		for (int i = 0; i < clist.size(); ++i) {
			auto c = clist.at(i);
			auto window = impl->buildCrosssectionWindow();
			window->internalWindow()->loadFromProjectMainFile(c);
			auto container = dynamic_cast<QWidget*> (window->parent());
			container->show();
			addCrossSectionWindow(window);
		}
	}
}

void PreProcessorGridDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("Shape");
	m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("NodeAttributes");
	m_nodeGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("CellAttributes");
	m_cellGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("IEdgeAttributes");
	m_iEdgeGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("JEdgeAttributes");
	m_jEdgeGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (m_bcGroupDataItem != nullptr) {
		writer.writeStartElement("BoundaryConditions");
		m_bcGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_birdEyeWindow != nullptr) {
		writer.writeStartElement("GridBirdEyeWindow");
		impl->m_birdEyeWindow->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	writer.writeStartElement("CrossSectionWindows");
	for (auto w : impl->m_crosssectionWindows) {
		writer.writeStartElement("CrossSectionWindow");
		w->internalWindow()->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

int PreProcessorGridDataItem::loadFromCgnsFile()
{
	auto gccItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (parent());
	auto zoneName = gccItem->zoneName();

	auto mainFile = dataModel()->iricMainWindow()->projectData()->mainfile();
	auto zone = mainFile->cgnsFile()->ccBase()->zone(zoneName);
	if (zone == nullptr) {return IRIC_NO_DATA;}

	return loadFromCgnsFile(*zone);
}

int PreProcessorGridDataItem::loadFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	auto gridType = gridTypeDataItem()->gridType();
	int ier;
	impl->m_grid = v4InputGridIO::load(zone, gridType, offset(), &ier);
	impl->m_grid->setGridDataItem(this);

	if (m_bcGroupDataItem != nullptr) {
		int ier = m_bcGroupDataItem->loadFromCgnsFile(zone);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	setDimensionsToAttributes();

	for (auto child : m_childItems) {
		int ier = child->loadFromCgnsFile();
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	impl->m_gridIsDeleted = false;

	// loading data finished.
	// now call vtk related functions, and render new grid.
	updateVtkObjectsForDrawing();
	updateActionStatus();
	updateObjectBrowserTree();

	for (auto w : impl->m_crosssectionWindows) {
		w->internalWindow()->applyTmpTargetSetting();
	}

	return IRIC_NO_ERROR;
}

int PreProcessorGridDataItem::saveToCgnsFile()
{
	auto gccItem = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (parent());
	auto zoneName = gccItem->zoneName();

	auto mainFile = dataModel()->iricMainWindow()->projectData()->mainfile();
	auto ccBase = mainFile->cgnsFile()->ccBase();

	return saveToCgnsFile(ccBase, zoneName);
}

int PreProcessorGridDataItem::saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName)
{
	if (impl->m_grid != nullptr) {
		iRICLib::H5CgnsZone* zone;
		int ier = v4InputGridIO::saveGrid(impl->m_grid, base, zoneName, offset(), &zone);
		if (ier != IRIC_NO_ERROR) {return ier;}

		impl->m_grid->setIsModified(false);
		if (m_bcGroupDataItem != nullptr) {
			try {
				m_bcGroupDataItem->saveToCgnsFile(zone);
			} catch (ErrorMessage& m) {
				impl->m_grid->setIsModified(true);
				return IRIC_H5_CALL_ERROR;
			}
		}
	}

	impl->m_gridIsDeleted = false;
	return IRIC_NO_ERROR;
}

void PreProcessorGridDataItem::closeCgnsFile()
{
	if (impl->m_grid != nullptr) {
		delete impl->m_grid;
		impl->m_grid = nullptr;
		m_shapeDataItem->informGridUpdate();
		m_nodeGroupDataItem->informGridUpdate();
		m_cellGroupDataItem->informGridUpdate();
		m_iEdgeGroupDataItem->informGridUpdate();
		m_jEdgeGroupDataItem->informGridUpdate();
		m_bcGroupDataItem->informGridUpdate();
	}
	updateRegionPolyData();
	updateObjectBrowserTree();
	updateActionStatus();
}

bool PreProcessorGridDataItem::importFromImporter(v4InputGrid* grid, GridImporterI* importer, const QString& filename, const QString& selectedFilter)
{
	auto projImporter = dynamic_cast<ProjectGridImporter*> (importer);
	if (projImporter != nullptr) {
		delete grid;
		ProjectImporter projectImporter2(this);
		return projectImporter2.importGrid(filename, projImporter);
	}
	auto cgnsImporter = dynamic_cast<CgnsGridImporter*> (importer);
	if (cgnsImporter != nullptr) {
		delete grid;
		CgnsImporter cgnsImporter2(this);
		return cgnsImporter2.importGrid(filename, cgnsImporter);
	}
	bool ok = importer->import(grid, filename, selectedFilter, projectData()->mainWindow());
	if (ok) {
		setGrid(grid);
		return true;
	} else {
		delete grid;
		return false;
	}
}

void PreProcessorGridDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_importAction);
	menu->addAction(impl->m_exportAction);

	menu->addSeparator();
	menu->addAction(impl->m_deleteAction);
}

void PreProcessorGridDataItem::exportGrid()
{
	// Check whether the grid shape is valid.
	QSettings settings;
	settings.beginGroup("gridcheck");

	QFile logFile(projectData()->absoluteFileName("gridcheck.txt"));
	logFile.open(QFile::WriteOnly | QFile::Text);
	QTextStream logStream(&logFile);
	if (settings.value("beforeexport", true).value<bool>()) {
		// execute checking before exporting.
		QStringList messages;
		auto grid2 = impl->m_grid->grid();
		bool ok = (grid2->structureChecker()->check(grid2, &messages, &logStream));
		if (! ok) {
			QString msg = tr("The following problems found in this grid. Do you really want to export the grid?");
			msg += "<ul>";
			for (int i = 0; i < messages.count(); ++i) {
				QString tmpmsg = messages.at(i);
				msg += "<li>" + tmpmsg + "</li>";
			}
			msg += "</ul>";
			QString logFileName = projectData()->absoluteFileName("gridcheck.txt");
			msg.append(QString("<a href=\"%1\">").arg(QString("file:///").append(logFileName))).append(tr("Show Detail")).append("</a>");
			int ret = QMessageBox::warning(mainWindow(), tr("Warning"), msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (ret == QMessageBox::No) {
				return;
			}
		}
	}
	logFile.close();
	QString dir = LastIODirectory::get();
	QString selectedFilter;
	QStringList filters;
	QList<GridExporterI*> exporters;

	auto exporterList = GridExporterFactory::instance().listForGridType(impl->m_grid->type());
	for (auto exporter : exporterList) {
		for (auto filter : exporter->fileDialogFilters()) {
			filters.append(filter);
			exporters.append(exporter);
		}
	}

	// Select the file to import.
	QString filename = QFileDialog::getSaveFileName(projectData()->mainWindow(), tr("Select File to Export"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()) {return;}
	GridExporterI* exporter = nullptr;
	for (int i = 0; i < filters.size(); ++i) {
		if (filters[i] == selectedFilter) {
			exporter = exporters[i];
		}
	}
	Q_ASSERT(exporter != nullptr);

	// execute export.
	projectData()->mainWindow()->statusBar()->showMessage(tr("Exporting Grid..."));
	bool ret;
	auto cgnsExporter = dynamic_cast<CgnsGridExporter*>(exporter);
	if (cgnsExporter != nullptr) {
		CgnsExporter exporter(this);
		ret = exporter.exportGrid(filename);
	} else {
		auto cs = projectData()->mainfile()->coordinateSystem();
		ret = exporter->doExport(impl->m_grid, filename, selectedFilter, cs, projectData()->mainWindow());
	}
	if (ret) {
		// exporting succeeded.
		projectData()->mainWindow()->statusBar()->showMessage(tr("Grid successfully exported to %1.").arg(QDir::toNativeSeparators(filename)), iRICMainWindowI::STATUSBAR_DISPLAYTIME);
	} else {
		// exporting failed.
		projectData()->mainWindow()->statusBar()->clearMessage();
		QMessageBox::critical(mainWindow(), tr("Error"), tr("Exporting grid to %1 failed.").arg(QDir::toNativeSeparators(filename)));
	}
	QFileInfo finfo(filename);
	LastIODirectory::set(finfo.absolutePath());
}

void PreProcessorGridDataItem::showDisplaySettingDialog()
{
	m_shapeDataItem->showPropertyDialog();
}

void PreProcessorGridDataItem::deleteGrid()
{
	if (impl->m_grid == nullptr) {return;}
	iRICMainWindowI* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	QMessageBox::StandardButton button = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to discard the grid?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (button == QMessageBox::No) {return;}
	silentDeleteGrid();
	renderGraphicsView();
}

v4InputGrid* PreProcessorGridDataItem::grid() const
{
	return impl->m_grid;
}

bool PreProcessorGridDataItem::setGrid(v4InputGrid* newGrid)
{
	newGrid->grid()->vtkData()->updateValueRangeSet();

	delete impl->m_grid;
	impl->m_grid = newGrid;
	impl->m_grid->setGridDataItem(this);

	setDimensionsToAttributes();

	PreProcessorGraphicsViewI* view = dataModel()->graphicsView();
	double xmin, xmax, ymin, ymax;
	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);

	auto grid2d = dynamic_cast<v4Grid2d*> (impl->m_grid->grid());
	grid2d->updateFilteredData(xmin, xmax, ymin, ymax);
	impl->m_grid->setIsModified(true);

	if (m_bcGroupDataItem != nullptr) {
		m_bcGroupDataItem->clearPoints();
	}

	// update vtk pipeline.
	updateVtkObjectsForDrawing();

	// update the object browser tree structure.
	updateObjectBrowserTree();
	updateActionStatus();
	m_shapeDataItem->updateActionStatus();
	informGridChange();
	return true;
}

void PreProcessorGridDataItem::informSelection(VTKGraphicsView* v)
{
	m_shapeDataItem->informSelection(v);
}

void PreProcessorGridDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

void PreProcessorGridDataItem::clearSelection()
{
	impl->m_selectedNodesController->clearSelection();
	impl->m_selectedCellsController->clearSelection();
	impl->m_selectedEdgesController->clearSelection();
}

void PreProcessorGridDataItem::mouseDoubleClickEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/)
{}

void PreProcessorGridDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_shapeDataItem->mouseMoveEvent(event, v);
}

void PreProcessorGridDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_shapeDataItem->mousePressEvent(event, v);
}

void PreProcessorGridDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_shapeDataItem->mouseReleaseEvent(event, v);
}

PreProcessorGridDataItem::SelectedNodesController* PreProcessorGridDataItem::selectedNodesController() const
{
	return impl->m_selectedNodesController;
}

PreProcessorGridDataItem::SelectedCellsController* PreProcessorGridDataItem::selectedCellsController() const
{
	return impl->m_selectedCellsController;
}

PreProcessorGridDataItem::SelectedIEdgesController* PreProcessorGridDataItem::selectedIEdgesController() const
{
	return impl->m_selectedIEdgesController;
}

PreProcessorGridDataItem::SelectedJEdgesController* PreProcessorGridDataItem::selectedJEdgesController() const
{
	return impl->m_selectedJEdgesController;
}

PreProcessorGridDataItem::SelectedEdgesController* PreProcessorGridDataItem::selectedEdgesController() const
{
	return impl->m_selectedEdgesController;
}

void PreProcessorGridDataItem::setupActors()
{
	vtkProperty* prop;

	impl->m_regionPolyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> tmppoints = vtkSmartPointer<vtkPoints>::New();
	impl->m_regionPolyData->SetPoints(tmppoints);

	impl->m_regionMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	impl->m_regionMapper->SetInputData(impl->m_regionPolyData);

	impl->m_regionActor = vtkSmartPointer<vtkActor>::New();
	impl->m_regionActor->SetMapper(impl->m_regionMapper);
	prop = impl->m_regionActor->GetProperty();
	prop->SetOpacity(0);
	prop->SetColor(0, 0, 0);
	impl->m_regionActor->VisibilityOff();
	renderer()->AddActor(impl->m_regionActor);
}

void PreProcessorGridDataItem::setupActions()
{
	PreProcessorGridAndGridCreatingConditionDataItem* gagcItem =
			dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (parent());
	impl->m_importAction = new QAction(tr("&Import..."), this);
	impl->m_importAction->setIcon(QIcon(":/libs/guibase/images/iconImport.svg"));
	connect(impl->m_importAction, &QAction::triggered, gagcItem, &PreProcessorGridAndGridCreatingConditionDataItem::importGrid);

	impl->m_exportAction = new QAction(tr("&Export..."), this);
	impl->m_exportAction->setIcon(QIcon(":/libs/guibase/images/iconExport.svg"));
	connect(impl->m_exportAction, &QAction::triggered, this, &PreProcessorGridDataItem::exportGrid);

	impl->m_displaySettingAction = new QAction(tr("Grid &Shape..."), this);
	connect(impl->m_displaySettingAction, &QAction::triggered, this, &PreProcessorGridDataItem::showDisplaySettingDialog);

	impl->m_polygonSelectAction = new QAction(tr("&Select Polygon Region"), this);

	impl->m_deleteAction = new QAction(tr("&Delete..."), this);
	impl->m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.svg"));
	connect(impl->m_deleteAction, &QAction::triggered, this, &PreProcessorGridDataItem::deleteGrid);

	impl->m_menu = new QMenu(tr("&Grid"));
	impl->m_nodeEditAction = new QAction(tr("&Node Attribute..."), this);
	impl->m_nodeDisplaySettingAction = new QAction(tr("&Node Attribute..."), this);

	impl->m_cellEditAction = new QAction(tr("&Cell Attribute..."), this);
	impl->m_cellDisplaySettingAction = new QAction(tr("&Cell Attribute..."), this);

	impl->m_birdEyeWindowAction = new QAction(tr("Open &Bird's-Eye View Window"), this);
	impl->m_birdEyeWindowAction->setIcon(QIcon(":/libs/pre/images/iconBirdEyeWindow.svg"));
	connect(impl->m_birdEyeWindowAction, &QAction::triggered, this, &PreProcessorGridDataItem::openBirdEyeWindow);

	impl->m_generateAttMenu = new QMenu(tr("Attributes &Generating"), mainWindow());
	setupGenerateAttributeActions(impl->m_generateAttMenu);
}

void PreProcessorGridDataItem::updateZDepthRangeItemCount()
{
	PreProcessorDataItem::updateZDepthRangeItemCount();
	m_zDepthRange.setItemCount(m_zDepthRange.itemCount() + 1);
}

void PreProcessorGridDataItem::informGridAttributeChangeAll()
{
	if (impl->m_grid == nullptr) {return;}

	for (auto att : impl->m_grid->attributes()) {
		informGridAttributeChange(att->name());
	}
}

void PreProcessorGridDataItem::informGridAttributeChange(const std::string& name)
{
	impl->m_grid->attribute(name)->updateValueRange();

	emit gridAttributeChanged(name);
	for (auto w : impl->m_crosssectionWindows) {
		w->update();
	}

	if (impl->m_birdEyeWindow != nullptr) {
		impl->m_birdEyeWindow->window()->updateGrid();
	}
}

void PreProcessorGridDataItem::updateVtkObjectsForDrawing()
{
	impl->m_selectedNodesController->handleDataUpdate();
	impl->m_selectedCellsController->handleDataUpdate();
	impl->m_selectedIEdgesController->handleDataUpdate();
	impl->m_selectedJEdgesController->handleDataUpdate();
	impl->m_selectedEdgesController->handleDataUpdate();

	updateSimplifiedGrid();
	// inform that all grid attributes are updated.
	informGridAttributeChangeAll();
	updateRegionPolyData();
}

bool PreProcessorGridDataItem::isImportAvailable()
{
	return true;
}

bool PreProcessorGridDataItem::isExportAvailable()
{
	if (impl->m_grid == nullptr) {return false;}
	auto exporterList = GridExporterFactory::instance().listForGridType(impl->m_grid->type());

	return exporterList.size() > 0;
}

bool PreProcessorGridDataItem::gridIsDeleted() const
{
	return impl->m_gridIsDeleted;
}

QAction* PreProcessorGridDataItem::importAction() const
{
	return impl->m_importAction;
}

QAction* PreProcessorGridDataItem::exportAction() const
{
	return impl->m_exportAction;
}

QAction* PreProcessorGridDataItem::displaySettingAction() const
{
	return impl->m_displaySettingAction;
}

QAction* PreProcessorGridDataItem::deleteAction() const
{
	return impl->m_deleteAction;
}

QAction* PreProcessorGridDataItem::polygonSelectAction() const
{
	return impl->m_polygonSelectAction;
}

QAction* PreProcessorGridDataItem::mappingAction() const
{
	PreProcessorGridAndGridCreatingConditionDataItem* item =
		dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent());
	PreProcessorGridAttributeMappingSettingTopDataItem* mItem = item->mappingSettingDataItem();
	return mItem->customMappingAction();
}

QAction* PreProcessorGridDataItem::nodeEditAction() const
{
	return impl->m_nodeEditAction;
}

QAction* PreProcessorGridDataItem::nodeDisplaySettingAction() const
{
	return impl->m_nodeDisplaySettingAction;
}

QAction* PreProcessorGridDataItem::cellEditAction() const
{
	return impl->m_cellEditAction;
}

QAction* PreProcessorGridDataItem::cellDisplaySettingAction() const
{
	return impl->m_cellDisplaySettingAction;
}

QAction* PreProcessorGridDataItem::birdEyeWindowAction() const
{
	return impl->m_birdEyeWindowAction;
}

QMenu* PreProcessorGridDataItem::generateAttMenu() const
{
	return impl->m_generateAttMenu;
}

QMenu* PreProcessorGridDataItem::menu() const
{
	return impl->m_menu;
}

void PreProcessorGridDataItem::updateActionStatus()
{
	impl->m_importAction->setEnabled(isImportAvailable());
	impl->m_exportAction->setEnabled(impl->m_grid != nullptr && isExportAvailable());

	impl->m_deleteAction->setEnabled(impl->m_grid != nullptr);
	impl->m_displaySettingAction->setEnabled(impl->m_grid != nullptr);
	impl->m_polygonSelectAction->setEnabled(impl->m_grid != nullptr);
	impl->m_birdEyeWindowAction->setEnabled(impl->m_grid != nullptr);

	impl->m_nodeEditAction->setEnabled((impl->m_grid != nullptr) && (impl->m_selectedNodesController->selectedDataIds().size() > 0) && (impl->m_nodeDataItem != nullptr));
	impl->m_nodeDisplaySettingAction->setEnabled((impl->m_grid != nullptr) && (impl->m_nodeDataItem != nullptr));
	impl->m_cellEditAction->setEnabled((impl->m_grid != nullptr) && (impl->m_selectedCellsController->selectedDataIds().size() > 0) && (impl->m_cellDataItem != nullptr));
	impl->m_cellDisplaySettingAction->setEnabled((impl->m_grid != nullptr) && (impl->m_cellDataItem != nullptr));

	m_shapeDataItem->editAction()->setEnabled(impl->m_grid != nullptr && impl->m_selectedNodesController->selectedDataIds().size() > 0);

	PreProcessorGridAttributeMappingSettingTopDataItem* mtItem =
		dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(parent())->mappingSettingDataItem();
	mtItem->customMappingAction()->setEnabled(impl->m_grid != nullptr);

	impl->m_generateAttMenu->setEnabled(impl->m_grid != nullptr && impl->m_grid->hasGeneratingAttributes());
}

void PreProcessorGridDataItem::silentDeleteGrid()
{
	if (impl->m_grid == nullptr) {return;}
	if (m_bcGroupDataItem != nullptr) {
		m_bcGroupDataItem->clearPoints();
	}
	delete impl->m_grid;
	impl->m_grid = nullptr;
	impl->m_gridIsDeleted = true;
	updateObjectBrowserTree();
	updateActionStatus();
	updateVtkObjectsForDrawing();
	clearSelection();
	closeBirdEyeWindow();
	auto gtItem = gridTypeDataItem();
	gtItem->geoDataTop()->clearDimensionsIfNoDataExists();

	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	pre->propertyBrowser()->view()->resetAttributes();

	iricMainWindow()->updateWindowList();

	iRICUndoStack::instance().clear();
}


PreProcessorGridTypeDataItem* PreProcessorGridDataItem::gridTypeDataItem() const
{
	return dynamic_cast<PreProcessorGridTypeDataItem*> (parent()->parent());
}

PreProcessorGridShapeDataItem* PreProcessorGridDataItem::shapeDataItem() const
{
	return m_shapeDataItem;
}

PreProcessorGridAttributeNodeGroupDataItem* PreProcessorGridDataItem::nodeGroupDataItem() const
{
	return m_nodeGroupDataItem;
}

PreProcessorGridAttributeCellGroupDataItem* PreProcessorGridDataItem::cellGroupDataItem() const
{
	return m_cellGroupDataItem;
}

PreProcessorGridAttributeIEdgeGroupDataItem* PreProcessorGridDataItem::iEdgeGroupDataItem() const
{
	return m_iEdgeGroupDataItem;
}

PreProcessorGridAttributeJEdgeGroupDataItem* PreProcessorGridDataItem::jEdgeGroupDataItem() const
{
	return m_jEdgeGroupDataItem;
}

PreProcessorBCGroupDataItem* PreProcessorGridDataItem::bcGroupDataItem() const
{
	return m_bcGroupDataItem;
}

bool PreProcessorGridDataItem::colorBarShouldBeVisible(const std::string& name) const
{
	if (m_nodeGroupDataItem->isChecked()) {
		return m_nodeGroupDataItem->colorBarShouldBeVisible(name);
	} else if (m_cellGroupDataItem->isChecked()) {
		return m_cellGroupDataItem->colorBarShouldBeVisible(name);
	} else if (m_iEdgeGroupDataItem->isChecked()) {
		return m_iEdgeGroupDataItem->colorBarShouldBeVisible(name);
	} else if (m_jEdgeGroupDataItem->isChecked()) {
		return m_jEdgeGroupDataItem->colorBarShouldBeVisible(name);
	}
	return false;
}

void PreProcessorGridDataItem::updateAttributeActorSettings()
{
	m_nodeGroupDataItem->updateActorSetting();
	m_cellGroupDataItem->updateActorSetting();
	m_iEdgeGroupDataItem->updateActorSetting();
	m_jEdgeGroupDataItem->updateActorSetting();
}

void PreProcessorGridDataItem::setNodeDataItem(PreProcessorGridAttributeNodeDataItem* nodeItem)
{
	impl->m_nodeDataItem = nodeItem;
}

void PreProcessorGridDataItem::setCellDataItem(PreProcessorGridAttributeCellDataItem* cellItem)
{
	impl->m_cellDataItem = cellItem;
}

QCursor PreProcessorGridDataItem::normalCursor()
{
	if (impl->m_shiftPressed) {
		return impl->m_addCursor;
	} else {
		return QCursor(Qt::ArrowCursor);
	}
}

void PreProcessorGridDataItem::openBirdEyeWindow()
{
	QWidget* w = nullptr;
	if (impl->m_birdEyeWindow == nullptr) {
		impl->m_birdEyeWindow = new GridBirdEyeWindowProjectDataItem(this, iricMainWindow());
		QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
		w = cent->addSubWindow(impl->m_birdEyeWindow->window());
		PreProcessorWindowI* pre = preProcessorWindow();
		QPoint p = pre->pos() + QPoint(10, 10);
		w->setWindowIcon(impl->m_birdEyeWindow->window()->icon());
		w->move(p);
		w->resize(640, 480);
	} else {
		w = dynamic_cast<QWidget*>(impl->m_birdEyeWindow->window()->parent());
	}
	w->show();
	w->setFocus();
	impl->m_birdEyeWindow->window()->cameraFit();
}

void PreProcessorGridDataItem::setDimensionsToAttributes()
{
	auto tItem = gridTypeDataItem();
	auto gdtItem = tItem->geoDataTop();
	for (auto att : impl->m_grid->attributes()) {
		auto gItem = gdtItem->groupDataItem(att->name());
		att->setDimensions(gItem->dimensions());
	}
}

void PreProcessorGridDataItem::closeBirdEyeWindow()
{
	if (impl->m_birdEyeWindow == nullptr) {return;}
	delete impl->m_birdEyeWindow;
	impl->m_birdEyeWindow = nullptr;
	
	auto w = iricMainWindow();
	if (w == nullptr) {return;}

	w->updateWindowList();
}

void PreProcessorGridDataItem::closeCrosssectionWindows()
{
	auto windows = impl->m_crosssectionWindows;
	for (auto w : windows) {
		delete w->parent();
	}

	auto w = iricMainWindow();
	if (w == nullptr) {return;}

	w->updateWindowList();
}

void PreProcessorGridDataItem::informGridChange()
{
	if (impl->m_birdEyeWindow != nullptr) {
		impl->m_birdEyeWindow->window()->updateGrid();
	}
	clearSelection();
}

void PreProcessorGridDataItem::informBirdEyeWindowClose()
{
	if (impl->m_birdEyeWindow == nullptr) {return;}

	delete impl->m_birdEyeWindow;
	impl->m_birdEyeWindow = nullptr;
}

void PreProcessorGridDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_regionActor->SetPosition(0, 0, range.min());
	impl->m_selectedNodesController->actor()->SetPosition(0, 0, range.max());
	impl->m_selectedCellsController->cellsActor()->SetPosition(0, 0, range.max());
	impl->m_selectedCellsController->cellsLinesActor()->SetPosition(0, 0, range.max());
	impl->m_selectedIEdgesController->cellsLinesActor()->SetPosition(0, 0, range.max());
	impl->m_selectedJEdgesController->cellsLinesActor()->SetPosition(0, 0, range.max());
	impl->m_selectedEdgesController->actor()->SetPosition(0, 0, range.max());

	ZDepthRange r;
	// Boundary Condition
	if (m_bcGroupDataItem != nullptr) {
		r = m_bcGroupDataItem->zDepthRange();
		r.setMax(range.max() * 0.9 + range.min() * 0.1);
		r.setMin(range.max() * 0.8 + range.min() * 0.2);
		m_bcGroupDataItem->setZDepthRange(r);
	}
	// Shape
	r = m_shapeDataItem->zDepthRange();
	r.setMax(range.max() * 0.7 + range.min() * 0.3);
	r.setMin(range.max() * 0.5 + range.min() * 0.5);
	m_shapeDataItem->setZDepthRange(r);

	// Node Condition
	r = m_nodeGroupDataItem->zDepthRange();
	r.setMax(range.max() * 0.4 + range.min() * 0.6);
	r.setMin(range.max() * 0.3 + range.min() * 0.7);
	m_nodeGroupDataItem->setZDepthRange(r);

	// Cell Condition
	r = m_cellGroupDataItem->zDepthRange();
	r.setMax(range.max() * 0.2 + range.min() * 0.8);
	r.setMin(range.max() * 0.1 + range.min() * 0.9);
	m_cellGroupDataItem->setZDepthRange(r);

	// IEdge Condition
	r = m_iEdgeGroupDataItem->zDepthRange();
	r.setMax(range.max() * 0.2 + range.min() * 0.8);
	r.setMin(range.max() * 0.1 + range.min() * 0.9);
	m_iEdgeGroupDataItem->setZDepthRange(r);

	// JEdge Condition
	r = m_jEdgeGroupDataItem->zDepthRange();
	r.setMax(range.max() * 0.2 + range.min() * 0.8);
	r.setMin(range.max() * 0.1 + range.min() * 0.9);
	m_jEdgeGroupDataItem->setZDepthRange(r);
}

vtkPolyData* PreProcessorGridDataItem::buildEdges() const
{
	if (impl->m_grid == nullptr) {return nullptr;}

	vtkPointSet* vtkGrid = impl->m_grid->grid()->vtkData()->data();
	vtkPolyData* polyData = vtkPolyData::New();
	polyData->SetPoints(vtkGrid->GetPoints());
	std::unordered_set<Edge, Edge::HashFunction> edges;

	for (vtkIdType i = 0; i < vtkGrid->GetNumberOfCells(); ++i) {
		vtkCell* cell = vtkGrid->GetCell(i);
		int edgeCount = cell->GetNumberOfEdges();
		for (int j = 0; j < edgeCount; ++j) {
			vtkCell* edgeCell = cell->GetEdge(j);
			Edge e(edgeCell->GetPointId(0), edgeCell->GetPointId(1));
			edges.insert(e);
		}
	}

	auto ca = vtkSmartPointer<vtkCellArray>::New();
	for (const auto& edge : edges) {
		vtkIdType nodes[2];
		nodes[0] = edge.vertex1();
		nodes[1] = edge.vertex2();
		ca->InsertNextCell(2, &(nodes[0]));
	}
	polyData->SetLines(ca);
	polyData->BuildCells();
	polyData->BuildLinks();

	return polyData;
}

void PreProcessorGridDataItem::updateObjectBrowserTree()
{
	QStandardItem* sItem = m_shapeDataItem->standardItem();
	if (sItem->row() != - 1) {
		// remove.
		m_standardItem->takeRow(sItem->row());
	}
	sItem = m_nodeGroupDataItem->standardItem();
	if (sItem->row() != - 1) {
		// remove.
		m_standardItem->takeRow(sItem->row());
	}
	sItem = m_cellGroupDataItem->standardItem();
	if (sItem->row() != - 1) {
		// remove.
		m_standardItem->takeRow(sItem->row());
	}
	sItem = m_iEdgeGroupDataItem->standardItem();
	if (sItem->row() != -1) {
		// remove.
		m_standardItem->takeRow(sItem->row());
	}
	sItem = m_jEdgeGroupDataItem->standardItem();
	if (sItem->row() != -1) {
		// remove.
		m_standardItem->takeRow(sItem->row());
	}

	if (m_bcGroupDataItem != nullptr) {
		sItem = m_bcGroupDataItem->standardItem();
		if (sItem->row() != - 1) {
			// remove.
			m_standardItem->takeRow(sItem->row());
		}
	}

	QString cap = tr("Grid");
	if (impl->m_grid == nullptr) {
		// do nothing.
		cap.append(tr(" [No Data]"));
		m_standardItem->setText(cap);
		m_nodeGroupDataItem->setTarget("");
		m_cellGroupDataItem->setTarget("");
		m_iEdgeGroupDataItem->setTarget("");
		m_jEdgeGroupDataItem->setTarget("");
	} else {
		vtkPointSet* ps = impl->m_grid->grid()->vtkData()->data();
		vtkStructuredGrid* sg = dynamic_cast<vtkStructuredGrid*>(ps);
		vtkUnstructuredGrid* ug = dynamic_cast<vtkUnstructuredGrid*>(ps);

		if (sg != nullptr) {
			int dim[3];
			sg->GetDimensions(dim);
			cap.append(tr(" (%1 x %2 = %3)").arg(dim[0]).arg(dim[1]).arg(dim[0] * dim[1]));
		} else if (ug != nullptr) {
			int num = ug->GetNumberOfPoints();
			cap.append(tr(" (%1)").arg(num));
		}

		m_standardItem->setText(cap);
		// add rows.
		// shape item.
		m_standardItem->appendRow(m_shapeDataItem->standardItem());
		// add node group if node data exists.
		if (m_nodeGroupDataItem->conditions().size() > 0) {
			m_standardItem->appendRow(m_nodeGroupDataItem->standardItem());
		}
		// add cell group if node data exists.
		if (m_cellGroupDataItem->conditions().size() > 0) {
			m_standardItem->appendRow(m_cellGroupDataItem->standardItem());
		}
		// add iedge group if node data exists.
		if (m_iEdgeGroupDataItem->conditions().size() > 0) {
			m_standardItem->appendRow(m_iEdgeGroupDataItem->standardItem());
		}
		// add jedge group if node data exists.
		if (m_jEdgeGroupDataItem->conditions().size() > 0) {
			m_standardItem->appendRow(m_jEdgeGroupDataItem->standardItem());
		}
		// add boundary condition group if boundary condition is defined.
		if (m_bcGroupDataItem) {
			m_standardItem->appendRow(m_bcGroupDataItem->standardItem());
		}
	}
}

void PreProcessorGridDataItem::setupGenerateAttributeActions(QMenu* menu)
{
	auto gtItem = gridTypeDataItem();
	for (SolverDefinitionGridAttribute* def : gtItem->gridType()->gridAttributes()) {
		if (def->mapping().isNull()) {continue;}

		QAction* action = new QAction(tr("Generate %1").arg(def->caption()), menu);
		action->setData(def->name().c_str());
		menu->addAction(action);
		connect(action, &QAction::triggered, this, &PreProcessorGridDataItem::launchAttributeGenerator);
	}
}

void PreProcessorGridDataItem::launchAttributeGenerator()
{
	QAction* action = dynamic_cast<QAction*>(sender());

	std::string attName = iRIC::toStr(action->data().toString());
	GridAttributeGeneratorLauncher::launchGenerator(this, attName, projectData()->workDirectory(), mainWindow());
}

std::vector<vtkIdType> PreProcessorGridDataItem::getCellsFromVertices(const std::unordered_set<vtkIdType>& vertices) const
{
	std::vector<vtkIdType> ret;
	if (impl->m_grid == nullptr) {return ret;}

	std::unordered_set<vtkIdType> selectedCellNoms;
	vtkPointSet* vtkGrid = impl->m_grid->grid()->vtkData()->data();
	auto idlist = vtkSmartPointer<vtkIdList>::New();
	for (vtkIdType v : vertices) {
		vtkGrid->GetPointCells(v, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType cellId = idlist->GetId(i);
			selectedCellNoms.insert(cellId);
		}
	}

	for (auto cellId : selectedCellNoms) {
		bool allSelected = true;
		vtkGrid->GetCellPoints(cellId, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType pointId = idlist->GetId(i);
			auto it = vertices.find(pointId);
			allSelected = allSelected && (it != vertices.end());
		}
		if (allSelected) {
			ret.push_back(cellId);
		}
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

std::vector<Edge> PreProcessorGridDataItem::getEdgesFromVertices(const std::unordered_set<vtkIdType>& vertices) const
{
	std::unordered_set<Edge, Edge::HashFunction> selectedEdgeNoms;

	vtkPolyData* pd = buildEdges();

	auto idlist = vtkSmartPointer<vtkIdList>::New();
	for (auto v : vertices) {
		pd->GetPointCells(v, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType cellId = idlist->GetId(i);
			vtkCell* cell = pd->GetCell(cellId);
			Edge edge(cell->GetPointId(0), cell->GetPointId(1));
			selectedEdgeNoms.insert(edge);
		}
	}

	std::vector<Edge> ret;
	for (const auto& edge : selectedEdgeNoms) {
		auto it1 = vertices.find(edge.vertex1());
		auto it2 = vertices.find(edge.vertex2());
		if (it1 != vertices.end() && it2 != vertices.end()) {
			ret.push_back(edge);
		}
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

std::vector<vtkIdType> PreProcessorGridDataItem::getIEdgesFromVertices(const std::unordered_set<vtkIdType>& vertices) const
{
	std::vector<vtkIdType> ret;
	if (impl->m_grid == nullptr) {return ret;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (impl->m_grid->grid());
	auto iEdgeGrid = sGrid->vtkIEdgeData()->data();
	std::unordered_set<vtkIdType> selectedEdgeNoms;

	auto idlist = vtkSmartPointer<vtkIdList>::New();
	for (auto v : vertices) {
		iEdgeGrid->GetPointCells(v, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType edgeId = idlist->GetId(i);
			selectedEdgeNoms.insert(edgeId);
		}
	}

	for (auto edgeId : selectedEdgeNoms) {
		bool allSelected = true;
		iEdgeGrid->GetCellPoints(edgeId, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType pointId = idlist->GetId(i);
			auto it = vertices.find(pointId);
			allSelected = allSelected && (it != vertices.end());
		}
		if (allSelected) {
			ret.push_back(edgeId);
		}
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

std::vector<vtkIdType> PreProcessorGridDataItem::getJEdgesFromVertices(const std::unordered_set<vtkIdType>& vertices) const
{
	std::vector<vtkIdType> ret;
	if (impl->m_grid == nullptr) {return ret;}

	auto sGrid = dynamic_cast<v4Structured2dGrid*> (impl->m_grid->grid());
	auto jEdgeGrid = sGrid->vtkJEdgeData()->data();
	std::unordered_set<vtkIdType> selectedEdgeNoms;

	auto idlist = vtkSmartPointer<vtkIdList>::New();
	for (auto v : vertices) {
		jEdgeGrid->GetPointCells(v, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType edgeId = idlist->GetId(i);
			selectedEdgeNoms.insert(edgeId);
		}
	}

	for (auto edgeId : selectedEdgeNoms) {
		bool allSelected = true;
		jEdgeGrid->GetCellPoints(edgeId, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType pointId = idlist->GetId(i);
			auto it = vertices.find(pointId);
			allSelected = allSelected && (it != vertices.end());
		}
		if (allSelected) {
			ret.push_back(edgeId);
		}
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

void PreProcessorGridDataItem::setBCGroupDataItem(PreProcessorBCGroupDataItem* item)
{
	if (item == 0) {return;}
	item->setParent(this);
	m_bcGroupDataItem = item;
	m_childItems.push_back(m_bcGroupDataItem);
}

void PreProcessorGridDataItem::unsetBCGroupDataItem()
{
	if (m_bcGroupDataItem == 0) {return;}
	auto it = std::find(m_childItems.begin(), m_childItems.end(), m_bcGroupDataItem);
	if (it != m_childItems.end()) {m_childItems.erase(it);}
	m_bcGroupDataItem->setParent(0);
	m_standardItem->takeChild(m_bcGroupDataItem->standardItem()->row());
	m_bcGroupDataItem = nullptr;
}

void PreProcessorGridDataItem::applyColorMapSetting(const std::string& name)
{
	m_nodeGroupDataItem->applyColorMapSetting(name);
	m_cellGroupDataItem->applyColorMapSetting(name);
	m_iEdgeGroupDataItem->applyColorMapSetting(name);
	m_jEdgeGroupDataItem->applyColorMapSetting(name);

	if (impl->m_birdEyeWindow != nullptr) {
		impl->m_birdEyeWindow->window()->updateGrid();
	}
	for (auto w : impl->m_crosssectionWindows) {
		w->internalWindow()->applyColorMapSetting(name);
	}
}

void PreProcessorGridDataItem::openCrossSectionWindow(PreProcessorGridCrosssectionInternalWindow::Direction dir, int index)
{
	auto window = impl->buildCrosssectionWindow();
	auto container = dynamic_cast<QWidget*> (window->parent());

	container->show();
	container->setFocus();
	window->internalWindow()->setTarget(dir, index);
	addCrossSectionWindow(window);
}

void PreProcessorGridDataItem::addCrossSectionWindow(PreProcessorGridCrosssectionWindow* w)
{
	impl->m_crosssectionWindows.push_back(w);
}

void PreProcessorGridDataItem::removeCrossSectionWindow(PreProcessorGridCrosssectionWindow* w)
{
	auto& windows = impl->m_crosssectionWindows;
	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (*it == w) {
			windows.erase(it);
			return;
		}
	}
}

void PreProcessorGridDataItem::doViewOperationEndedGlobal(VTKGraphicsView* v)
{
	updateSimplifiedGrid(v);
}

void PreProcessorGridDataItem::updateSimplifiedGrid(VTKGraphicsView* v)
{
	if (impl->m_grid != nullptr) {
		if (v == nullptr) {
			v = dataModel()->graphicsView();
		}
		auto view = dynamic_cast<PreProcessorGraphicsView*>(v);
		double xmin, xmax, ymin, ymax;
		view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);

		auto grid2d = dynamic_cast<v4Grid2d*> (impl->m_grid->grid());
		grid2d->updateFilteredData(xmin, xmax, ymin, ymax);
	}

	m_shapeDataItem->informGridUpdate();
	m_nodeGroupDataItem->informGridUpdate();
	m_cellGroupDataItem->informGridUpdate();
	m_iEdgeGroupDataItem->informGridUpdate();
	m_jEdgeGroupDataItem->informGridUpdate();
}

void PreProcessorGridDataItem::updateRegionPolyData()
{
	if (impl->m_grid == nullptr) {
		impl->m_regionPolyData->Reset();
		auto points = vtkSmartPointer<vtkPoints>::New();
		impl->m_regionPolyData->SetPoints(points);
		return;
	}
	double bounds[6];
	impl->m_grid->grid()->vtkData()->data()->GetBounds(bounds);

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(4);
	points->InsertNextPoint(bounds[0], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[3], 0);
	points->InsertNextPoint(bounds[0], bounds[3], 0);
	impl->m_regionPolyData->SetPoints(points);

	vtkIdType pts[4] = {0, 1, 2, 3};
	auto cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(4, pts);
	impl->m_regionPolyData->SetPolys(cells);
	impl->m_regionPolyData->Modified();

	actorCollection()->RemoveItem(impl->m_regionActor);
	actorCollection()->AddItem(impl->m_regionActor);
	updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::renderGraphicsView()
{
	GraphicsWindowDataItem::renderGraphicsView();
}

void PreProcessorGridDataItem::doApplyOffset(double x, double y)
{
	if (impl->m_grid == nullptr) {return;}

	impl->m_grid->grid()->applyOffset(QPointF(x, y));
	impl->m_grid->setIsModified(true);

	this->updateRegionPolyData();
}
