#include "../factory/gridexporterfactory.h"
#include "../factory/gridimporterfactory.h"
#include "../gridexporter/cgnsgridexporter.h"
#include "../gridimporter/cgnsgridimporter.h"
#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "../subwindow/gridbirdeyewindow/gridbirdeyewindow.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributemappingsettingtopdataitem.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "../misc/gridattributegeneratorlauncher.h"
#include "private/preprocessorgriddataitem_impl.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid2d.h>
#include <guicore/pre/grid/gridcgnsestimater.h>
#include <guicore/pre/grid/gridexporterinterface.h>
#include <guicore/pre/grid/gridimporterinterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/errormessage.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QIcon>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSet>
#include <QSettings>
#include <QSignalMapper>
#include <QStandardItem>
#include <QStatusBar>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QtGlobal>

#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

PreProcessorGridDataItem::Impl::Impl() :
	m_grid {nullptr},
	m_nodeDataItem {nullptr},
	m_cellDataItem {nullptr},
	m_birdEyeWindow {nullptr},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_menu {nullptr},
	m_generateAttMenu {nullptr},
	m_shiftPressed {false},
	m_gridIsDeleted {false}
{}

PreProcessorGridDataItem::Impl::~Impl()
{
	delete m_grid;
	delete m_menu;
	delete m_generateAttMenu;
}


PreProcessorGridDataItem::PreProcessorGridDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridDataItemInterface(parent),
	m_bcGroupDataItem {nullptr},
	impl {new Impl {}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	// Set cursors for mouse view change events.
	setupActors();
	setupActions();
}

PreProcessorGridDataItem::~PreProcessorGridDataItem()
{
	renderer()->RemoveActor(impl->m_regionActor);
	renderer()->RemoveActor(impl->m_selectedVerticesActor);
	renderer()->RemoveActor(impl->m_selectedCellsActor);
	renderer()->RemoveActor(impl->m_selectedCellsLinesActor);
	renderer()->RemoveActor(impl->m_selectedEdgesActor);
	closeBirdEyeWindow();
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

	QDomNode bcNode = iRIC::getChildNode(node, "BoundaryConditions");
	if (! bcNode.isNull() && m_bcGroupDataItem != nullptr) {
		m_bcGroupDataItem->loadFromProjectMainFile(bcNode);
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

	if (m_bcGroupDataItem != nullptr) {
		writer.writeStartElement("BoundaryConditions");
		m_bcGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
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
	impl->m_grid = GridCgnsEstimater::buildGrid(zone, nullptr);
	SolverDefinitionGridType* gridType = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent())->gridType();
	gridType->buildGridAttributes(impl->m_grid);

	impl->m_grid->setParent(this);
	impl->m_grid->setDataItem(this);
	impl->m_grid->setZoneName(zone.name());

	// Now, memory is allocated. load data.
	impl->m_grid->loadFromCgnsFile(zone);

	if (m_bcGroupDataItem != nullptr) {
		int ier = m_bcGroupDataItem->loadFromCgnsFile(zone);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		int ier = (*it)->loadFromCgnsFile();
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	impl->m_gridIsDeleted = false;

	// loading data finished.
	// now call vtk related functions, and render new grid.
	updateSimplifiedGrid();
	finishGridLoading();
	updateActionStatus();
	updateObjectBrowserTree();

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
		impl->m_grid->saveToCgnsFile(base, zoneName);
		impl->m_grid->setModified(false);
		if (m_bcGroupDataItem != nullptr) {
			try {
				auto zone = base->zone(zoneName);
				m_bcGroupDataItem->saveToCgnsFile(zone);
			} catch (ErrorMessage& m) {
				impl->m_grid->setModified();
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
		m_bcGroupDataItem->informGridUpdate();
	}
	updateRegionPolyData();
	updateObjectBrowserTree();
	updateActionStatus();
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
		QStringList messages = impl->m_grid->checkShape(logStream);
		if (messages.count() > 0) {
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
	QList<GridExporterInterface*> exporters;

	const QList<GridExporterInterface*> exporterList = GridExporterFactory::instance().list(impl->m_grid->gridType());
	for (auto it = exporterList.begin(); it != exporterList.end(); ++it) {
		QStringList flist = (*it)->fileDialogFilters();
		for (auto fit = flist.begin(); fit != flist.end(); ++fit) {
			filters.append(*fit);
			exporters.append(*it);
		}
	}

	// Select the file to import.
	QString filename = QFileDialog::getSaveFileName(projectData()->mainWindow(), tr("Select File to Export"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()) {return;}
	GridExporterInterface* exporter = nullptr;
	for (int i = 0; i < filters.count(); ++i) {
		if (filters[i] == selectedFilter) {
			exporter = exporters[i];
		}
	}
	Q_ASSERT(exporter != nullptr);

	// execute export.
	projectData()->mainWindow()->statusBar()->showMessage(tr("Exporting Grid..."));
	bool ret;
	CgnsGridExporter* cgnsExporter = dynamic_cast<CgnsGridExporter*>(exporter);
	if (cgnsExporter != nullptr) {
		// CGNS exporter is a little special.
		// Boundary condition should be exported too.
		auto tmpname = projectData()->tmpFileName();
		try {
			ret = true;
			auto file = new iRICLib::H5CgnsFile(iRIC::toStr(tmpname), iRICLib::H5CgnsFile::Mode::Create);
			impl->m_grid->saveToCgnsFile(file->ccBase(), "iRICZone");
			auto zone = file->ccBase()->zone("IRICZone");
			m_bcGroupDataItem->saveToCgnsFile(zone);
			delete file;

			// if it already exists, remove it first.
			QFile cgnsfile(filename);
			if (cgnsfile.exists()) {
				cgnsfile.remove();
			}
			// copy to the specified file.
			bool ok = QFile::copy(tmpname, filename);
			if (! ok) {ret = false;}

			// Delete the temporary file
			QFile::remove(tmpname);
		}  catch (...) {
			ret = false;
		}
	} else {
		auto cs = projectData()->mainfile()->coordinateSystem();
		ret = exporter->doExport(impl->m_grid, filename, selectedFilter, cs, projectData()->mainWindow());
	}
	if (ret) {
		// exporting succeeded.
		projectData()->mainWindow()->statusBar()->showMessage(tr("Grid successfully exported to %1.").arg(QDir::toNativeSeparators(filename)), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
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
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	QMessageBox::StandardButton button = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to discard the grid?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (button == QMessageBox::No) {return;}
	silentDeleteGrid();
	renderGraphicsView();
}

Grid* PreProcessorGridDataItem::grid() const
{
	return impl->m_grid;
}

bool PreProcessorGridDataItem::setGrid(Grid* newGrid)
{
	delete impl->m_grid;
	impl->m_grid = newGrid;
	// set parent.
	impl->m_grid->setParent(this);
	impl->m_grid->setDataItem(this);
	PreProcessorGraphicsViewInterface* view = dataModel()->graphicsView();
	double xmin, xmax, ymin, ymax;
	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	impl->m_grid->updateSimplifiedGrid(xmin, xmax, ymin, ymax);
	impl->m_grid->setModified();
	// set zone name.
	std::string zname = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(parent())->zoneName();
	impl->m_grid->setZoneName(zname);
	if (m_bcGroupDataItem != nullptr) {
		m_bcGroupDataItem->clearPoints();
	}

	// update vtk pipeline.
	finishGridLoading();

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

void PreProcessorGridDataItem::setSelectedPointsVisibility(bool visible)
{
	impl->m_selectedVerticesActor->VisibilityOff();
	m_actorCollection->RemoveItem(impl->m_selectedVerticesActor);
	if (visible) {
		m_actorCollection->AddItem(impl->m_selectedVerticesActor);
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::setSelectedCellsVisibility(bool visible)
{
	impl->m_selectedCellsActor->VisibilityOff();
	impl->m_selectedCellsLinesActor->VisibilityOff();
	m_actorCollection->RemoveItem(impl->m_selectedCellsActor);
	m_actorCollection->RemoveItem(impl->m_selectedCellsLinesActor);
	if (visible) {
		m_actorCollection->AddItem(impl->m_selectedCellsActor);
		m_actorCollection->AddItem(impl->m_selectedCellsLinesActor);
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::setSelectedEdgesVisibility(bool visible)
{
	impl->m_selectedEdgesActor->VisibilityOff();
	m_actorCollection->RemoveItem(impl->m_selectedEdgesActor);
	if (visible) {
		m_actorCollection->AddItem(impl->m_selectedEdgesActor);
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::informDeselection(VTKGraphicsView* v)
{
	m_shapeDataItem->informDeselection(v);
}

void PreProcessorGridDataItem::clearSelection()
{
	impl->m_selectedVertices.clear();
	impl->m_selectedCells.clear();
	impl->m_selectedEdges.clear();

	updateSelectedVerticesGraphics();
	updateSelectedCellsGraphics();
	updateSelectedEdgesGraphics();
}

void PreProcessorGridDataItem::updateSelectedVertices(MouseBoundingBox* box, bool xOr)
{
	impl->m_selectedVertices = getSelectedVertices(box, xOr);

	updateSelectedVerticesGraphics();

	m_shapeDataItem->updateActionStatus();
	updateActionStatus();
	informSelectedVerticesChanged();
}

void PreProcessorGridDataItem::updateSelectedVerticesGraphics()
{
	impl->m_selectedVerticesPolyData->Reset();
	if (impl->m_grid == nullptr) {
		impl->m_selectedVerticesPolyData->BuildCells();
		impl->m_selectedVerticesPolyData->BuildLinks();
		impl->m_selectedVerticesPolyData->Modified();
		return;
	}
	impl->m_selectedVerticesPolyData->SetPoints(impl->m_grid->vtkGrid()->GetPoints());
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType node;
	for (int i = 0; i < impl->m_selectedVertices.count(); ++i) {
		node = impl->m_selectedVertices.at(i);
		cells->InsertNextCell(1, &(node));
	}
	impl->m_selectedVerticesPolyData->SetVerts(cells);
	impl->m_selectedVerticesPolyData->BuildCells();
	impl->m_selectedVerticesPolyData->BuildLinks();
	impl->m_selectedVerticesPolyData->Modified();
}

QVector<vtkIdType> PreProcessorGridDataItem::getSelectedVertices(MouseBoundingBox* box, bool xOr)
{
	QVector<vtkIdType> ret;
	if (impl->m_grid == nullptr) {
		// grid is not setup yet.
		return ret;
	}
//	if (impl->m_grid->isMasked()){return ret;}
	QSet<vtkIdType> selectedVerticesSet;
	if (xOr) {
		for (vtkIdType i = 0; i < impl->m_selectedVertices.count(); ++i) {
			selectedVerticesSet.insert(impl->m_selectedVertices.at(i));
		}
	}

	vtkPoints* points = impl->m_grid->vtkGrid()->GetPoints();
	double p[3];
	Structured2DGrid* sgrid = dynamic_cast<Structured2DGrid*>(impl->m_grid);
	if (sgrid != nullptr) {
		for (int ii = sgrid->drawnIMin(); ii <= sgrid->drawnIMax(); ++ii) {
			for (int jj = sgrid->drawnJMin(); jj <= sgrid->drawnJMax(); ++jj) {
				vtkIdType idx = sgrid->vertexIndex(ii, jj);
				points->GetPoint(idx, p);
				if (box->isInsideBox(p[0], p[1])) {
					if (selectedVerticesSet.contains(idx)) {
						selectedVerticesSet.remove(idx);
					} else {
						selectedVerticesSet.insert(idx);
					}
				}
			}
		}
	} else {
		vtkIdType i;
		for (i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			if (box->isInsideBox(p[0], p[1])) {
				if (selectedVerticesSet.contains(i)) {
					selectedVerticesSet.remove(i);
				} else {
					selectedVerticesSet.insert(i);
				}
			}
		}
	}

	for (auto s_it = selectedVerticesSet.begin(); s_it != selectedVerticesSet.end(); ++s_it) {
		ret.append(*s_it);
	}
	return ret;
}


QSet<vtkIdType> PreProcessorGridDataItem::getSelectedVerticesSet(MouseBoundingBox* box, bool xOr)
{
	QVector<vtkIdType> vec = getSelectedVertices(box, xOr);
	QSet<vtkIdType> set;
	for (int i = 0; i < vec.size(); ++i) {
		set.insert(vec.at(i));
	}
	return set;
}

void PreProcessorGridDataItem::setSelectedVertices(const QVector<vtkIdType>& vertices)
{
	impl->m_selectedVertices = vertices;
	updateSelectedVerticesGraphics();
}

void PreProcessorGridDataItem::informSelectedVerticesChanged()
{
	m_nodeGroupDataItem->informSelectedVerticesChanged(impl->m_selectedVertices);
}

void PreProcessorGridDataItem::updateSelectedCells(MouseBoundingBox* box, bool xOr)
{
	if (impl->m_grid == nullptr) {
		// grid is not setup yet.
		return;
	}
//	if (impl->m_grid->isMasked()){return;}

	bool click = false;
	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		int r = iRIC::nearRadius();
		QPoint newStart = box->endPoint() - QPoint(r, r);
		QPoint newEnd = box->endPoint() + QPoint(r, r);
		box->setStartPoint(newStart.x(), newStart.y());
		box->setEndPoint(newEnd.x(), newEnd.y());
		click = true;
	}

	QSet<vtkIdType> selectedCellsSet;
	if (xOr) {
		for (vtkIdType i = 0; i < impl->m_selectedCells.count(); ++i) {
			selectedCellsSet.insert(impl->m_selectedCells.at(i));
		}
	}
	impl->m_selectedCells.clear();

	QVector<vtkIdType> selectedCellsVector;

	if (click) {
		double point[3];
		box->vtkGrid()->GetPoint(0, point);
		// find the cell that contains point.
		vtkCell* hintCell = nullptr;
		double pcoords[4];
		double weights[4];
		int subid;
		vtkIdType cellid = impl->m_grid->vtkGrid()->FindCell(point, hintCell, 0, 1e-4, subid, pcoords, weights);
		if (cellid >= 0) {
			selectedCellsVector.append(cellid);
		}
	} else {
		// setup selectedVertices.
		QSet<vtkIdType> selectedVertices = getSelectedVerticesSet(box, false);
		selectedCellsVector = getCellsFromVertices(selectedVertices);
	}
	for (int i = 0; i < selectedCellsVector.count(); ++i) {
		vtkIdType cellid = selectedCellsVector.at(i);
		if (selectedCellsSet.contains(cellid)) {
			selectedCellsSet.remove(cellid);
		} else {
			selectedCellsSet.insert(cellid);
		}
	}

	for (auto s_it = selectedCellsSet.begin(); s_it != selectedCellsSet.end(); ++s_it) {
		impl->m_selectedCells.append(*s_it);
	}
	updateSelectedCellsGraphics();
	updateActionStatus();
}

void PreProcessorGridDataItem::updateSelectedCellsGraphics()
{

	vtkSmartPointer<vtkIdList> cellids = vtkSmartPointer<vtkIdList>::New();
	for (int i = 0; i < impl->m_selectedCells.count(); ++i) {
		cellids->InsertNextId(impl->m_selectedCells[i]);
	}
	impl->m_selectedCellsGrid->SetCellList(cellids);

	impl->m_selectedCellsGrid->Modified();
	m_shapeDataItem->updateActionStatus();
}

void PreProcessorGridDataItem::updateSelectedEdges(MouseBoundingBox* box, bool xOr, VTKGraphicsView* v)
{
	if (impl->m_grid == nullptr) {
		// grid is not setup yet.
		return;
	}
//	if (impl->m_grid->isMasked()){return;}

	bool click = false;
	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		QPoint center;
		center.setX((box->startPoint().x() + box->endPoint().x()) / 2);
		center.setY((box->startPoint().y() + box->endPoint().y()) / 2);
		box->setStartPoint(center.x(), center.y());
		box->setEndPoint(center.x(), center.y());
		click = true;
	}

	QSet<Edge> selectedEdgesSet;
	if (xOr) {
		for (int i = 0; i < impl->m_selectedEdges.count(); ++i) {
			selectedEdgesSet.insert(impl->m_selectedEdges.at(i));
		}
	}

	QVector<Edge> selectedEdgesVector;
	if (click) {
		double p[3];
		VTK2DGraphicsView* v2 = dynamic_cast<VTK2DGraphicsView*>(v);
		double stdDist = v2->stdRadius(iRIC::nearRadius());
		vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();
		box->vtkGrid()->GetPoint(0, p);
		vtkPolyData* pd = buildEdges();
		vtkIdType pointId = pd->FindPoint(p);
		pd->GetPointCells(pointId, cellIds);
		for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i) {
			vtkCell* cell = pd->GetCell(cellIds->GetId(i));
			Edge edge(cell->GetPointId(0), cell->GetPointId(1));
			// check whether cell is clicked.
			double tmpp[3];
			pd->GetPoint(cell->GetPointId(0), tmpp);
			QPointF v0(tmpp[0], tmpp[1]);
			pd->GetPoint(cell->GetPointId(1), tmpp);
			QPointF v1(tmpp[0], tmpp[1]);

			QPointF horizontal = v1 - v0;
			QPointF vertical = iRIC::normalize(v1 - v0) * stdDist;
			vertical = iRIC::rotateVector90(vertical);
			QPointF posv = v0 - vertical * 0.5;
			QPointF point(p[0], p[1]);
			if (iRIC::isInsideParallelogram(point, posv, horizontal, vertical)) {
				selectedEdgesVector.append(edge);
				break;
			}
		}
		pd->Delete();
	} else {
		// setup selectedVertices.
		QSet<vtkIdType> selectedVertices = getSelectedVerticesSet(box, false);
		selectedEdgesVector = getEdgesFromVertices(selectedVertices);
	}
	for (int i = 0; i < selectedEdgesVector.count(); ++i) {
		const Edge& e = selectedEdgesVector.at(i);
		if (selectedEdgesSet.contains(e)) {
			selectedEdgesSet.remove(e);
		} else {
			selectedEdgesSet.insert(e);
		}
	}

	impl->m_selectedEdges.clear();
	for (auto s_it = selectedEdgesSet.begin(); s_it != selectedEdgesSet.end(); ++s_it) {
		impl->m_selectedEdges.append(*s_it);
	}

	updateSelectedEdgesGraphics();

	m_shapeDataItem->updateActionStatus();
	updateActionStatus();
}

void PreProcessorGridDataItem::updateSelectedEdgesGraphics()
{
	impl->m_selectedEdgesPolyData->Reset();
	if (impl->m_grid == nullptr) {
		return;
	}
	impl->m_selectedEdgesPolyData->SetPoints(impl->m_grid->vtkGrid()->GetPoints());
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType nodes[2];
	for (int i = 0; i < impl->m_selectedEdges.count(); ++i) {
		const Edge& edge = impl->m_selectedEdges.at(i);
		nodes[0] = edge.vertex1();
		nodes[1] = edge.vertex2();
		cells->InsertNextCell(2, &(nodes[0]));
	}

	impl->m_selectedEdgesPolyData->SetLines(cells);
	impl->m_selectedEdgesPolyData->Modified();
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

void PreProcessorGridDataItem::nodeSelectingMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	// drawing bounding box using mouse dragging.
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	renderGraphicsView();
}

void PreProcessorGridDataItem::nodeSelectingMousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->enable();

	v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	renderGraphicsView();
}

void PreProcessorGridDataItem::nodeSelectingMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		int r = iRIC::nearRadius();
		QPoint newStart = box->endPoint() - QPoint(r, r);
		QPoint newEnd = box->endPoint() + QPoint(r, r);
		box->setStartPoint(newStart.x(), newStart.y());
		box->setEndPoint(newEnd.x(), newEnd.y());
	}

	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

	updateSelectedVertices(box, xOr);
	box->disable();

	v->restoreUpdateRate();
	renderGraphicsView();
}

void PreProcessorGridDataItem::nodeSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
		v->setCursor(impl->m_addCursor);
		impl->m_shiftPressed = true;
	} else {
		impl->m_shiftPressed = false;
	}
}

void PreProcessorGridDataItem::nodeSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
		v->setCursor(impl->m_addCursor);
		impl->m_shiftPressed = true;
	} else {
		v->unsetCursor();
		impl->m_shiftPressed = false;
	}
}

void PreProcessorGridDataItem::cellSelectingMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	// drawing bounding box using mouse dragging.
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	renderGraphicsView();
}

void PreProcessorGridDataItem::cellSelectingMousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->enable();

	v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	renderGraphicsView();
}

void PreProcessorGridDataItem::cellSelectingMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());
	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

	updateSelectedCells(box, xOr);
	box->disable();

	v->restoreUpdateRate();
	renderGraphicsView();
}

void PreProcessorGridDataItem::cellSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	nodeSelectingKeyPressEvent(event, v);
}

void PreProcessorGridDataItem::cellSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	nodeSelectingKeyReleaseEvent(event, v);
}

void PreProcessorGridDataItem::edgeSelectingMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	// drawing bounding box using mouse dragging.
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	renderGraphicsView();
}

void PreProcessorGridDataItem::edgeSelectingMousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->enable();

	v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	renderGraphicsView();
}

void PreProcessorGridDataItem::edgeSelectingMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
//	if (impl->m_grid != nullptr && impl->m_grid->isMasked()){return;}
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());
	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

	updateSelectedEdges(box, xOr, v);
	box->disable();

	v->restoreUpdateRate();
	renderGraphicsView();
}

void PreProcessorGridDataItem::edgeSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	nodeSelectingKeyPressEvent(event, v);
}

void PreProcessorGridDataItem::edgeSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	nodeSelectingKeyReleaseEvent(event, v);
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

	impl->m_selectedVerticesPolyData = vtkSmartPointer<vtkPolyData>::New();

	impl->m_selectedVerticesActor = vtkSmartPointer<vtkActor>::New();
	prop = impl->m_selectedVerticesActor->GetProperty();
	prop->SetPointSize(5);
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0);
	prop->SetRepresentationToPoints();
	renderer()->AddActor(impl->m_selectedVerticesActor);

	impl->m_selectedVerticesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	impl->m_selectedVerticesMapper->SetScalarVisibility(false);
	impl->m_selectedVerticesActor->SetMapper(impl->m_selectedVerticesMapper);
	impl->m_selectedVerticesMapper->SetInputData(impl->m_selectedVerticesPolyData);

	impl->m_selectedVerticesActor->VisibilityOff();

	impl->m_selectedCellsGrid = vtkSmartPointer<vtkExtractCells>::New();

	impl->m_selectedCellsActor = vtkSmartPointer<vtkActor>::New();
	prop = impl->m_selectedCellsActor->GetProperty();
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0);
	prop->SetOpacity(0.5);
	prop->SetRepresentationToSurface();

	renderer()->AddActor(impl->m_selectedCellsActor);

	impl->m_selectedCellsMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	impl->m_selectedCellsMapper->SetScalarVisibility(false);
	impl->m_selectedCellsActor->SetMapper(impl->m_selectedCellsMapper);
	impl->m_selectedCellsMapper->SetInputConnection(impl->m_selectedCellsGrid->GetOutputPort());

	impl->m_selectedCellsLinesActor = vtkSmartPointer<vtkActor>::New();
	prop = impl->m_selectedCellsLinesActor->GetProperty();
	prop->SetLighting(false);
	prop->SetLineWidth(3);
	prop->SetColor(0, 0, 0);
	prop->SetRepresentationToWireframe();

	renderer()->AddActor(impl->m_selectedCellsLinesActor);

	impl->m_selectedCellsLinesMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	impl->m_selectedCellsLinesMapper->SetScalarVisibility(false);
	impl->m_selectedCellsLinesActor->SetMapper(impl->m_selectedCellsLinesMapper);
	impl->m_selectedCellsLinesMapper->SetInputConnection(impl->m_selectedCellsGrid->GetOutputPort());

	impl->m_selectedCellsActor->VisibilityOff();
	impl->m_selectedCellsLinesActor->VisibilityOff();

	impl->m_selectedEdgesPolyData = vtkSmartPointer<vtkPolyData>::New();

	impl->m_selectedEdgesActor = vtkSmartPointer<vtkActor>::New();
	prop = impl->m_selectedEdgesActor->GetProperty();
	prop->SetLighting(false);
	prop->SetLineWidth(3);
	prop->SetColor(0, 0, 0);

	renderer()->AddActor(impl->m_selectedEdgesActor);

	impl->m_selectedEdgesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	impl->m_selectedEdgesMapper->SetScalarVisibility(false);
	impl->m_selectedEdgesActor->SetMapper(impl->m_selectedEdgesMapper);
	impl->m_selectedEdgesMapper->SetInputData(impl->m_selectedEdgesPolyData);

	impl->m_selectedEdgesActor->VisibilityOff();
}

void PreProcessorGridDataItem::setupActions()
{
	PreProcessorGridAndGridCreatingConditionDataItem* gagcItem =
			dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (parent());
	impl->m_importAction = new QAction(tr("&Import..."), this);
	impl->m_importAction->setIcon(QIcon(":/libs/guibase/images/iconImport.svg"));
	connect(impl->m_importAction, SIGNAL(triggered()), gagcItem, SLOT(importGrid()));

	impl->m_exportAction = new QAction(tr("&Export..."), this);
	impl->m_exportAction->setIcon(QIcon(":/libs/guibase/images/iconExport.svg"));
	connect(impl->m_exportAction, SIGNAL(triggered()), this, SLOT(exportGrid()));

	impl->m_displaySettingAction = new QAction(tr("Grid &Shape..."), this);
	connect(impl->m_displaySettingAction, SIGNAL(triggered()), this, SLOT(showDisplaySettingDialog()));

	impl->m_polygonSelectAction = new QAction(tr("&Select Polygon Region"), this);

	impl->m_deleteAction = new QAction(tr("&Delete..."), this);
	impl->m_deleteAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.svg"));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteGrid()));

	impl->m_menu = new QMenu(tr("&Grid"));
	impl->m_nodeEditAction = new QAction(tr("&Node Attribute..."), this);
	impl->m_nodeDisplaySettingAction = new QAction(tr("&Node Attribute..."), this);

	impl->m_cellEditAction = new QAction(tr("&Cell Attribute..."), this);
	impl->m_cellDisplaySettingAction = new QAction(tr("&Cell Attribute..."), this);

	impl->m_setupScalarBarAction = new QAction(tr("Set &Up Scalarbar..."), this);
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	connect(impl->m_setupScalarBarAction, SIGNAL(triggered()), gtItem->geoDataTop(), SLOT(setupScalarBar()));

	impl->m_birdEyeWindowAction = new QAction(tr("Open &Bird's-Eye View Window"), this);
	impl->m_birdEyeWindowAction->setIcon(QIcon(":/libs/pre/images/iconBirdEyeWindow.svg"));
	connect(impl->m_birdEyeWindowAction, SIGNAL(triggered()), this, SLOT(openBirdEyeWindow()));

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
	QList<GridAttributeContainer*> conds = impl->m_grid->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		informGridAttributeChange((*it)->name());
	}
}

void PreProcessorGridDataItem::informGridAttributeChange(const std::string& name)
{
	emit gridAttributeChanged(name);
	auto nItem = m_nodeGroupDataItem->nodeDataItem(name);
	if (nItem != nullptr) {nItem->updateCrossectionWindows();}
}

void PreProcessorGridDataItem::finishGridLoading()
{
	if (impl->m_grid != nullptr) {
		impl->m_selectedVerticesPolyData->SetPoints(impl->m_grid->vtkGrid()->GetPoints());
		impl->m_selectedCellsGrid->SetInputData(impl->m_grid->vtkGrid());
		impl->m_selectedEdgesPolyData->SetPoints(impl->m_grid->vtkGrid()->GetPoints());
	} else {
		impl->m_selectedVerticesPolyData->SetPoints(nullptr);
		impl->m_selectedCellsGrid->SetInputData(nullptr);
		impl->m_selectedEdgesPolyData->SetPoints(nullptr);
	}
	// inform that all grid attributes are updated.
	informGridAttributeChangeAll();
	// update vtk pipeline.
	m_shapeDataItem->informGridUpdate();
	m_nodeGroupDataItem->informGridUpdate();
	m_cellGroupDataItem->informGridUpdate();
	updateRegionPolyData();
}

bool PreProcessorGridDataItem::isImportAvailable()
{
	return true;
}

bool PreProcessorGridDataItem::isExportAvailable()
{
	if (impl->m_grid == nullptr) {return false;}
	const QList<GridExporterInterface*> exporterList = GridExporterFactory::instance().list(impl->m_grid->gridType());
	return exporterList.count() > 0;
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

QAction* PreProcessorGridDataItem::setupScalarBarAction() const
{
	return impl->m_setupScalarBarAction;
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

vtkPolyData* PreProcessorGridDataItem::selectedVerticesPolyData() const
{
	return impl->m_selectedVerticesPolyData;
}

const QVector<vtkIdType>& PreProcessorGridDataItem::selectedVertices() const
{
	return impl->m_selectedVertices;
}

const QVector<vtkIdType>& PreProcessorGridDataItem::selectedCells() const
{
	return impl->m_selectedCells;
}

const QVector<Edge>& PreProcessorGridDataItem::selectedEdges() const
{
	return impl->m_selectedEdges;
}

void PreProcessorGridDataItem::updateActionStatus()
{
	impl->m_importAction->setEnabled(isImportAvailable());
	impl->m_exportAction->setEnabled(impl->m_grid != nullptr && isExportAvailable());

	impl->m_deleteAction->setEnabled(impl->m_grid != nullptr);
	impl->m_displaySettingAction->setEnabled(impl->m_grid != nullptr);
	impl->m_polygonSelectAction->setEnabled(impl->m_grid != nullptr);
	impl->m_birdEyeWindowAction->setEnabled(impl->m_grid != nullptr);

	impl->m_nodeEditAction->setEnabled((impl->m_grid != nullptr) && (impl->m_selectedVertices.count() > 0) && (impl->m_nodeDataItem != nullptr));
	impl->m_nodeDisplaySettingAction->setEnabled((impl->m_grid != nullptr) && (impl->m_nodeDataItem != nullptr));
	impl->m_cellEditAction->setEnabled((impl->m_grid != nullptr) && (impl->m_selectedCells.count() > 0) && (impl->m_cellDataItem != nullptr));
	impl->m_cellDisplaySettingAction->setEnabled((impl->m_grid != nullptr) && (impl->m_cellDataItem != nullptr));

	impl->m_setupScalarBarAction->setEnabled(impl->m_grid != nullptr);

	m_shapeDataItem->editAction()->setEnabled(impl->m_grid != nullptr && impl->m_selectedVertices.count() > 0);

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
	finishGridLoading();
	clearSelection();
	closeBirdEyeWindow();
	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	gtItem->geoDataTop()->clearDimensionsIfNoDataExists();
	iricMainWindow()->updateWindowList();

	iRICUndoStack::instance().clear();
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

PreProcessorBCGroupDataItem* PreProcessorGridDataItem::bcGroupDataItem() const
{
	return m_bcGroupDataItem;
}

void PreProcessorGridDataItem::updateAttributeActorSettings()
{
	m_nodeGroupDataItem->updateActorSettings();
	m_cellGroupDataItem->updateActorSettings();
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
	QWidget* w;
	if (impl->m_birdEyeWindow == nullptr) {
		impl->m_birdEyeWindow = new GridBirdEyeWindow(iricMainWindow(), this);
		QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
		w = cent->addSubWindow(impl->m_birdEyeWindow);
		PreProcessorWindowInterface* pre = preProcessorWindow();
		QPoint p = pre->pos() + QPoint(10, 10);
		w->setWindowIcon(impl->m_birdEyeWindow->icon());
		w->move(p);
		w->resize(640, 480);
	} else {
		w = dynamic_cast<QWidget*>(impl->m_birdEyeWindow->parent());
	}
	w->show();
	w->setFocus();
	impl->m_birdEyeWindow->cameraFit();
}

void PreProcessorGridDataItem::closeBirdEyeWindow()
{
	if (impl->m_birdEyeWindow == nullptr) {return;}
	delete impl->m_birdEyeWindow->parent();
	
	iricMainWindow()->updateWindowList();
}

void PreProcessorGridDataItem::informGridChange()
{
	if (impl->m_birdEyeWindow != nullptr) {
		impl->m_birdEyeWindow->updateGrid();
	}
	clearSelection();
}

void PreProcessorGridDataItem::informBirdEyeWindowClose()
{
	impl->m_birdEyeWindow = nullptr;
}


void PreProcessorGridDataItem::assignActorZValues(const ZDepthRange& range)
{
	// selected.
	impl->m_regionActor->SetPosition(0, 0, range.min());
	impl->m_selectedVerticesActor->SetPosition(0, 0, range.max());
	impl->m_selectedCellsActor->SetPosition(0, 0, range.max());
	impl->m_selectedCellsLinesActor->SetPosition(0, 0, range.max());

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
}

vtkPolyData* PreProcessorGridDataItem::buildEdges() const
{
	if (impl->m_grid == nullptr) {return nullptr;}
	vtkPolyData* polyData = vtkPolyData::New();
	polyData->SetPoints(impl->m_grid->vtkGrid()->GetPoints());
	QSet<Edge> edges;

	for (vtkIdType i = 0; i < impl->m_grid->vtkGrid()->GetNumberOfCells(); ++i) {
		vtkCell* cell = impl->m_grid->vtkGrid()->GetCell(i);
		int edgeCount = cell->GetNumberOfEdges();
		for (int j = 0; j < edgeCount; ++j) {
			vtkCell* edgeCell = cell->GetEdge(j);
			Edge e(edgeCell->GetPointId(0), edgeCell->GetPointId(1));
			edges.insert(e);
		}
	}

	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	for (auto it = edges.begin(); it != edges.end(); ++it) {
		vtkIdType nodes[2];
		nodes[0] = it->vertex1();
		nodes[1] = it->vertex2();
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
	if (m_bcGroupDataItem) {
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
	} else {
		vtkPointSet* ps = impl->m_grid->vtkGrid();
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
		if (m_nodeGroupDataItem->conditions().count() > 0) {
			m_standardItem->appendRow(m_nodeGroupDataItem->standardItem());
		}
		// add cell group if node data exists.
		if (m_cellGroupDataItem->conditions().count() > 0) {
			m_standardItem->appendRow(m_cellGroupDataItem->standardItem());
		}
		// add boundary condition group if boundary condition is defined.
		if (m_bcGroupDataItem) {
			m_standardItem->appendRow(m_bcGroupDataItem->standardItem());
		}
	}
}

void PreProcessorGridDataItem::setupGenerateAttributeActions(QMenu* menu)
{
	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItem*> (parent()->parent());
	for (SolverDefinitionGridAttribute* def : gtItem->gridType()->gridAttributes()) {
		if (def->mapping().isNull()) {continue;}

		QAction* action = new QAction(tr("Generate %1").arg(def->caption()), menu);
		action->setData(def->name().c_str());
		menu->addAction(action);
		connect(action, SIGNAL(triggered()), this, SLOT(launchAttributeGenerator()));
	}
}

void PreProcessorGridDataItem::launchAttributeGenerator()
{
	QAction* action = dynamic_cast<QAction*>(sender());

	std::string attName = iRIC::toStr(action->data().toString());
	GridAttributeGeneratorLauncher::launchGenerator(this, attName, projectData()->workDirectory(), mainWindow());
}

QVector<vtkIdType> PreProcessorGridDataItem::getCellsFromVertices(const QSet<vtkIdType>& vertices) const
{
	QSet<vtkIdType> selectedCellNoms;
	vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();
	for (auto v_it = vertices.begin(); v_it != vertices.end(); ++v_it) {
		impl->m_grid->vtkGrid()->GetPointCells(*v_it, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType cellid = idlist->GetId(i);
			selectedCellNoms.insert(cellid);
		}
	}

	QVector<vtkIdType> ret;
	for (auto v_it = selectedCellNoms.begin(); v_it != selectedCellNoms.end(); ++v_it) {
		bool allSelected = true;
		impl->m_grid->vtkGrid()->GetCellPoints(*v_it, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType pointid = idlist->GetId(i);
			allSelected = allSelected && vertices.contains(pointid);
		}
		if (allSelected) {
			ret.append(*v_it);
		}
	}
	qSort(ret);
	return ret;
}

QVector<Edge> PreProcessorGridDataItem::getEdgesFromVertices(const QSet<vtkIdType>& vertices) const
{
	QSet<Edge> selectedEdgeNoms;

	vtkPolyData* pd = buildEdges();

	vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();
	for (auto v_it = vertices.begin(); v_it != vertices.end(); ++v_it) {
		pd->GetPointCells(*v_it, idlist);
		for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
			vtkIdType cellid = idlist->GetId(i);
			vtkCell* cell = pd->GetCell(cellid);
			Edge edge(cell->GetPointId(0), cell->GetPointId(1));
			selectedEdgeNoms.insert(edge);
		}
	}

	QVector<Edge> ret;
	for (auto e_it = selectedEdgeNoms.begin(); e_it != selectedEdgeNoms.end(); ++e_it) {
		if (vertices.contains(e_it->vertex1()) && vertices.contains(e_it->vertex2())) {
			ret.append(*e_it);
		}
	}
	qSort(ret);
	return ret;
}

void PreProcessorGridDataItem::setBCGroupDataItem(PreProcessorBCGroupDataItem* item)
{
	if (item == 0) {return;}
	item->setParent(this);
	m_bcGroupDataItem = item;
	m_standardItem->appendRow(m_bcGroupDataItem->standardItem());
	m_childItems.push_back(m_bcGroupDataItem);
}

void PreProcessorGridDataItem::unsetBCGroupDataItem()
{
	if (m_bcGroupDataItem == 0) {return;}
	auto it = std::find(m_childItems.begin(), m_childItems.end(), m_bcGroupDataItem);
	if (it != m_childItems.end()) {m_childItems.erase(it);}
	m_bcGroupDataItem->setParent(0);
	m_standardItem->takeChild(m_bcGroupDataItem->standardItem()->row());
	m_bcGroupDataItem = 0;
}

void PreProcessorGridDataItem::doViewOperationEndedGlobal(VTKGraphicsView* v)
{
	updateSimplifiedGrid(v);
}

void PreProcessorGridDataItem::updateSimplifiedGrid(VTKGraphicsView* v)
{
	if (impl->m_grid == nullptr) {return;}
	if (v == nullptr) {
		v = dataModel()->graphicsView();
	}
	PreProcessorGraphicsView* view = dynamic_cast<PreProcessorGraphicsView*>(v);
	double xmin, xmax, ymin, ymax;
	view->getDrawnRegion(&xmin, &xmax, &ymin, &ymax);
	impl->m_grid->updateSimplifiedGrid(xmin, xmax, ymin, ymax);

	m_shapeDataItem->informGridUpdate();
	m_nodeGroupDataItem->informGridUpdate();
	m_cellGroupDataItem->informGridUpdate();
}

void PreProcessorGridDataItem::updateRegionPolyData()
{
	Grid* grid = this->grid();
	if (grid == nullptr) {
		impl->m_regionPolyData->Reset();
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		impl->m_regionPolyData->SetPoints(points);
		return;
	}
	double bounds[6];
	grid->vtkGrid()->GetBounds(bounds);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(4);
	points->InsertNextPoint(bounds[0], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[2], 0);
	points->InsertNextPoint(bounds[1], bounds[3], 0);
	points->InsertNextPoint(bounds[0], bounds[3], 0);
	impl->m_regionPolyData->SetPoints(points);

	vtkIdType pts[4] = {0, 1, 2, 3};
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
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
	vtkPoints* points = impl->m_grid->vtkGrid()->GetPoints();
	vtkIdType numPoints = points->GetNumberOfPoints();
	for (vtkIdType id = 0; id < numPoints; ++id) {
		double v[3];
		points->GetPoint(id, v);
		v[0] -= x;
		v[1] -= y;
		points->SetPoint(id, v);
	}
	points->Modified();
	impl->m_grid->setModified();
	this->updateRegionPolyData();
}
