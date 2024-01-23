#include "geodatapointmap.h"
#include "geodatapointmapmappingmode.h"
#include "geodatapointmapmergesettingdialog.h"
#include "geodatapointmapproxy.h"
#include "geodatapointmaptemplatemappingsetting.h"
#include "geodatapointmaptemplatenodemapper.h"
#include "private/geodatapointmap_impl.h"
#include "private/geodatapointmap_mappingsettingdialog.h"
#include "private/geodatapointmap_modifycommand.h"
#include "private/geodatapointmap_tinmanager_breakline.h"
#include "public/geodatapointmap_displaysettingwidget.h"

#include <geodata/polygongroup/geodatapolygongroup.h>
#include <guibase/polygon/polygonpushvertexcommand.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/modifycommanddialog.h>
#include <misc/qpointfcompare.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>
#include <triangle/triangle.h>
#include <triangle/triangleexecutethread.h>

#include <QFile>
#include <QToolBar>
#include <QXmlStreamWriter>

#include <vtkLODActor.h>
#include <vtkMapperCollection.h>
#include <vtkProperty.h>
#include <vtkVersionMacros.h>

#include <iriclib_pointmap.h>

namespace {

void applyOffsetToPoints(vtkPoints* points, double x, double y)
{
	double v[3];
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		v[0] -= x;
		v[1] -= y;
		points->SetPoint(i, v);
	}
	points->Modified();
}

} // namespace

const char* GeoDataPointmap::VALUES = "values";

GeoDataPointmap::GeoDataPointmap(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData {d, creator, att},
	// m_longEdgeRemover {nullptr},
	impl {new Impl {this}}
{
	auto values = vtkSmartPointer<vtkDoubleArray>::New();
	values->SetName("values");
	impl->m_pointsManager.points()->GetPointData()->AddArray(values);

	m_templateMapper = new GeoDataPointmapTemplateNodeMapper(creator);

	impl->setupActions();
}

GeoDataPointmap::~GeoDataPointmap()
{
	vtkRenderer* r = renderer();
	impl->m_pointsManager.removeActorsFromRenderer(r);
	impl->m_tinManager.removeActorsFromRenderer(r);

	delete impl;
}

bool GeoDataPointmap::getValueRange(double* min, double* max)
{
	if (pointsValues()->GetNumberOfTuples() == 0) {return false;}

	double range[2];
	pointsValues()->GetRange(range);
	*min = range[0];
	*max = range[1];
	return true;
}

GeoDataMapper* GeoDataPointmap::mapper() const
{
	if (impl->m_mappingSetting.mappingMode == MappingSetting::MappingMode::TIN) {
		if (m_gridAttribute->position() == SolverDefinitionGridAttribute::Position::Node) {
			return &impl->m_tinNodeMapper;
		} else {
			return &impl->m_tinCellMapper;
		}
	} else if (impl->m_mappingSetting.mappingMode == MappingSetting::MappingMode::Template) {
		if (m_gridAttribute->position() == SolverDefinitionGridAttribute::Position::Node) {
			return &impl->m_templateNodeMapper;
		} else {
			return &impl->m_templateCellMapper;
		}
	} else if (impl->m_mappingSetting.mappingMode == MappingSetting::MappingMode::Polygons) {
		if (m_gridAttribute->position() == SolverDefinitionGridAttribute::Position::Node) {
			return &impl->m_polygonsNodeMapper;
		} else {
			return &impl->m_polygonsCellMapper;
		}
	}
}

vtkPolyData* GeoDataPointmap::points() const
{
	return impl->m_pointsManager.points();
}

vtkDoubleArray* GeoDataPointmap::pointsValues() const
{
	return impl->m_pointsManager.values();
}

vtkPolyData* GeoDataPointmap::tin() const
{
	return impl->m_tinManager.tin();
}

vtkDoubleArray* GeoDataPointmap::tinValues() const
{
	return impl->m_tinManager.values();
}

void GeoDataPointmap::loadExternalData(const QString& filename)
{
	if (projectData()->version().build() >= 3607) {
		iRICLib::PointMap pm;
		pm.load(iRIC::toStr(filename).c_str());
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		points->Allocate(pm.pointCount);

		for (int i = 0; i < pm.pointCount; ++i) {
			points->InsertNextPoint(pm.x.at(i), pm.y.at(i), 0);
		}
		points->Modified();

		auto da = vtkSmartPointer<vtkDoubleArray>::New();
		for (int i = 0; i < pm.pointCount; ++i) {
			da->InsertNextValue(pm.realValue.at(i));
		}

		impl->m_pointsManager.setPoints(points, da);

		auto tin = vtkSmartPointer<vtkPolyData>::New();
		tin->SetPoints(points);

		auto ca = vtkSmartPointer<vtkCellArray>::New();
		vtkIdType pts[3];
		for (unsigned int i = 0; i < pm.triangles.size(); ++i) {
			const iRICLib::PointMapTriangle& tri = pm.triangles.at(i);
			pts[0] = tri.index1 - 1;
			pts[1] = tri.index2 - 1;
			pts[2] = tri.index3 - 1;
			ca->InsertNextCell(3, pts);
		}
		tin->SetPolys(ca);

		impl->m_tinManager.setTinData(tin, da);

		auto r = renderer();
		auto points2 = impl->m_pointsManager.points();
		for (const auto& bl : pm.breaklines) {
			auto line = new TINManager::BreakLine(points2);
			line->addActorsToRenderer(r);

			std::vector<vtkIdType> indices;
			for (int index : bl.indices) {
				indices.push_back(index);
			}
			line->setVertexIndices(indices);
			impl->m_tinManager.breakLines().push_back(line);
		}

		QString polygons_filename = filename;
		polygons_filename.replace(".dat", "_polygons.dat");
		if (QFile::exists(polygons_filename)) {
			impl->m_polygonsManager.loadPolygons(polygons_filename);
		}

	} else {
		// load data from the external file.
		QFileInfo finfo(filename);

		QString vtkFilename = finfo.absolutePath().append(QString("/%1.vtk").arg(finfo.baseName()));
		QFile f(vtkFilename);
		f.open(QIODevice::ReadOnly);
		QTextStream stream(&f);

		// read 4th line.
		QString line;
		for (int i = 0; i < 4; ++i) {
			line = stream.readLine();
		}
		if (line == "DATASET UNSTRUCTURED_GRID") {
			// Saved as UNSTRUCTURED GRID. backward compatibility.
			vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New();
			reader->SetFileName(iRIC::toStr(filename).c_str());
			reader->Update();
			auto grid = reader->GetOutput();
			auto points = grid->GetPoints();
			auto values = vtkDoubleArray::SafeDownCast(grid->GetPointData()->GetArray(VALUES));
			impl->m_pointsManager.setPoints(points, values);
			reader->Delete();
		} else {
			// POLYDATA. new data.
			vtkPolyDataReader* reader = vtkPolyDataReader::New();
			reader->SetFileName(iRIC::toStr(vtkFilename).c_str());
			reader->Update();
			auto polydata = reader->GetOutput();
			auto points = polydata->GetPoints();
			auto values = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray(VALUES));
			impl->m_pointsManager.setPoints(points, values);
			reader->Delete();
		}
		impl->m_pointsManager.points()->Modified();

		// load delaunayed poly data.
		QString delFilename = finfo.absolutePath().append(QString("/%1_del.vtk").arg(finfo.baseName()));
		QFile delFile(delFilename);
		if (delFile.exists()) {
			vtkPolyDataReader* reader = vtkPolyDataReader::New();
			reader->SetFileName(iRIC::toStr(delFilename).c_str());
			reader->Update();
			auto tin = reader->GetOutput();
			auto values = vtkDoubleArray::SafeDownCast(reader->GetOutput()->GetPointData()->GetArray(VALUES));
			impl->m_tinManager.setTinData(tin, values);
			reader->Delete();
		} else {
			rebuildTinFromPoints(false);
		}

		// load break lines
		QString breaklinesFilename = finfo.absolutePath().append(QString("/%1_breaklines.dat").arg(finfo.baseName()));
		QFile breaklinesFile(breaklinesFilename);
		if (breaklinesFile.exists()) {
			breaklinesFile.open(QIODevice::ReadOnly);
			QDataStream s(&breaklinesFile);
			int num;
			s >> num;
			auto r = renderer();
			auto points2 = impl->m_pointsManager.points();
			for (int i = 0; i < num; ++i) {
				auto line = new TINManager::BreakLine(points2);
				line->addActorsToRenderer(r);
				QVector<vtkIdType> indices;
				s >> indices;
				std::vector<vtkIdType> indices2;
				for (auto index : indices) {
					indices2.push_back(index);
				}
				line->setVertexIndices(indices2);
				impl->m_tinManager.breakLines().push_back(line);
			}
		}
		breaklinesFile.close();
	}

	updateActorSetting();
}

void GeoDataPointmap::saveExternalData(const QString& filename)
{
	iRICLib::PointMap* pm = new iRICLib::PointMap();

	vtkPoints* points = impl->m_tinManager.tin()->GetPoints();
	vtkDoubleArray* values = impl->m_tinManager.values();

	vtkIdType pnum = points->GetNumberOfPoints();
	pm->x.reserve(pnum);
	pm->y.reserve(pnum);
	for (vtkIdType i = 0; i < pnum; ++i) {
		double point[3];
		points->GetPoint(i, point);
		pm->x.push_back(point[0]);
		pm->y.push_back(point[1]);
	}

	pm->valueType = iRICLib::PointMap::vtReal;
	pm->realValue.reserve(pnum);
	for (vtkIdType i = 0; i < pnum; ++i) {
		double value = values->GetValue(i);
		pm->realValue.push_back(value);
	}

	vtkIdType tnum = impl->m_tinManager.tin()->GetNumberOfCells();
	pm->triangles.reserve(tnum);
	for (vtkIdType i = 0; i < tnum; ++i) {
		vtkCell* cell = impl->m_tinManager.tin()->GetCell(i);
		iRICLib::PointMapTriangle tri;
		tri.index1 = cell->GetPointId(0) + 1;
		tri.index2 = cell->GetPointId(1) + 1;
		tri.index3 = cell->GetPointId(2) + 1;
		pm->triangles.push_back(tri);
	}
	pm->breaklines.reserve(impl->m_tinManager.breakLines().size());
	for (auto line : impl->m_tinManager.breakLines()) {
		auto indices = line->vertexIndices();
		iRICLib::PointMapBreakline bl;
		for (auto index : indices) {
			bl.indices.push_back(index);
		}
		pm->breaklines.push_back(bl);
	}

	pm->save(iRIC::toStr(filename).c_str());
	delete pm;

	QString polygons_filename = filename;
	polygons_filename.replace(".dat", "_polygons.dat");

	impl->m_polygonsManager.savePolygons(polygons_filename);
}

void GeoDataPointmap::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPointmap::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);

	impl->m_tinManager.load(node);
	impl->m_displaySetting.load(node);
	impl->m_mappingSetting.load(node);

	auto mcmNode = iRIC::getChildNode(node, "MappingColorMap");
	if (! mcmNode.isNull()) {
		impl->m_polygonsManager.polygonsColorMap()->load(mcmNode);
	}
}

void GeoDataPointmap::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	rebuildTinFromPointsIfNeeded();

	GeoData::doSaveToProjectMainFile(writer);

	impl->m_tinManager.save(writer);
	impl->m_displaySetting.save(writer);
	impl->m_mappingSetting.save(writer);

	writer.writeStartElement("MappingColorMap");
	impl->m_polygonsManager.polygonsColorMap()->save(writer);
	writer.writeEndElement();
}

bool GeoDataPointmap::getValueAt(double x, double y, double* value)
{
	rebuildTinFromPointsIfNeeded();

	double weights[3];
	vtkCell* cell = findTinCell(x, y, weights);

	if (cell == nullptr) {return false;}

	double v = 0;
	auto values = impl->m_tinManager.values();

	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		vtkIdType vid = cell->GetPointId(i);
		v += *(weights + i) * values->GetValue(vid);
	}
	*value = v;
	return true;
}

bool GeoDataPointmap::mapWithPolygons(double x, double y, double* value)
{
	return impl->m_polygonsManager.map(x, y, value);
}

bool GeoDataPointmap::needRebuildTin() const
{
	return impl->m_tinManager.needRebuild();
}

void GeoDataPointmap::setNeedRebuildTin(bool needed)
{
	impl->m_tinManager.setNeedRebuild(needed);
}

void GeoDataPointmap::rebuildTinFromPointsIfNeeded()
{
	impl->m_tinManager.rebuildTinFromPointsIfNeeded();
}

bool GeoDataPointmap::rebuildTinFromPoints(bool allowCancel)
{
	return impl->m_tinManager.rebuildTinFromPoints(allowCancel);
}

void GeoDataPointmap::mergeEditTargetPolygon()
{
	impl->m_polygonsManager.mergeEditTargetPolygon();
}

void GeoDataPointmap::buildPointsFromTIN()
{
	auto points = impl->m_tinManager.tin()->GetPoints();
	auto values = impl->m_tinManager.values();

	impl->m_pointsManager.setPoints(points, values);
}

void GeoDataPointmap::setupActors()
{
	auto r = renderer();
	impl->m_pointsManager.addActorsToRenderer(r);
	impl->m_tinManager.addActorsToRenderer(r);

	updateActorSetting();
}

void GeoDataPointmap::setupMenu()
{
	m_menu->setTitle(tr("P&oint Cloud Data"));

	updateMenu();
}

bool GeoDataPointmap::addToolBarButtons(QToolBar* tb)
{
	/*
	tb->addAction(impl->m_selectionModePoint);
	tb->addAction(impl->m_selectionModePolygon);
	tb->addSeparator();

	tb->addAction(impl->m_addPointAction);
	tb->addAction(impl->m_interpolatePointAction);
	tb->addSeparator();
	tb->addAction(impl->m_addBreakLineAction);
	tb->addAction(impl->m_removeBreakLineAction);
	*/
	return false;
}

void GeoDataPointmap::applyColorMapSetting()
{
	updateActorSetting();
}

void GeoDataPointmap::setPoints(vtkPoints* points, vtkDoubleArray* values)
{
	impl->m_pointsManager.setPoints(points, values);

	updateActorSetting();
}

void GeoDataPointmap::setTin(vtkPolyData* data, vtkDoubleArray* values)
{
	impl->m_tinManager.setTinData(data, values);

	buildPointsFromTIN();
	updateActorSetting();
}

void GeoDataPointmap::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GeoDataPointmap::assignActorZValues(const ZDepthRange& range)
{
	m_zDepthRange = range;

	double selectionPolygonDepth = range.max();
	double breakLineDepth = range.max() * 0.9 + range.min() * 0.1;
	double pointsDepth = range.max() * 0.5 + range.min() * 0.5;
	double polygonsDepthMax = range.max() * 0.3 + range.min() * 0.7;
	double polygonsDepthMin = range.max() * 0.2 + range.min() * 0.8;
	double selectedPointsDepth = range.min();
	double tinDepth = range.min();

	impl->m_pointsManager.setSelectionPolygonZDepth(selectionPolygonDepth);
	impl->m_tinManager.setZDepthToBreakLines(breakLineDepth);
	impl->m_pointsManager.pointsActor()->SetPosition(0, 0, pointsDepth);
	ZDepthRange polygonsRange;
	polygonsRange.setRange(polygonsDepthMin, polygonsDepthMax);
	impl->m_polygonsManager.polygonGroup()->assignActorZValues(polygonsRange);
	impl->m_pointsManager.selectedPointsActor()->SetPosition(0, 0, selectedPointsDepth);
	impl->m_tinManager.tinActor()->SetPosition(0, 0, tinDepth);
}

void GeoDataPointmap::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPointmap::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoDataGroupDataItem()->condition()->name(), parent);
	auto widget = new DisplaySettingWidget(dialog);
	widget->setSetting(&impl->m_displaySetting);

	if (! geoDataGroupDataItem()->condition()->isReferenceInformation()) {
		auto colorMapWidget = geoDataGroupDataItem()->condition()->createColorMapSettingEditWidget(widget);
		auto colormap = geoDataDataItem()->colorMapSettingContainer();
		colorMapWidget->setSetting(colormap);
		auto colorMapWidget2 = new ColorMapSettingEditWidgetWithImportExportButton(colorMapWidget, widget);

		widget->setColorMapWidget(colorMapWidget2);
	}

	auto polygonColorMapWidget = impl->m_polygonsManager.attribute()->createColorMapSettingEditWidget(widget);
	auto polygonColorMap = impl->m_polygonsManager.polygonsColorMap();
	polygonColorMapWidget->setSetting(polygonColorMap);
	auto polygonColorMapWidget2 = new ColorMapSettingEditWidgetWithImportExportButton(polygonColorMapWidget, widget);
	widget->setPolygonColorMapWidget(polygonColorMapWidget2);

	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Point Cloud Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataPointmap::updateMenu()
{
	m_menu->clear();
	m_menu->addAction(m_editNameAction);
	m_menu->addSeparator();
	updateMenu(m_menu);

	impl->m_rightClickingMenu->clear();
	updateMenu(impl->m_rightClickingMenu);
}

void GeoDataPointmap::updateMenu(QMenu* menu)
{
	impl->m_activeController->addActionsToMenu(menu);
	menu->addSeparator();
	menu->addMenu(impl->m_modeMenu);
	menu->addSeparator();
	menu->addAction(impl->m_mergeAction);
	menu->addAction(impl->m_mappingSettingAction);
	menu->addAction(impl->m_displaySettingAction);
}

void GeoDataPointmap::updateActorSetting()
{
	auto v = dataModel()->graphicsView();

	actorCollection()->RemoveAllItems();

	impl->m_pointsManager.pointsActor()->VisibilityOff();
	impl->m_tinManager.tinActor()->VisibilityOff();

	auto cs = colorMapSettingContainer();
	// tin
	if (impl->m_activeController == &impl->m_pointsManager) {
		// point edit mode
		auto mapper = cs->buildPointDataMapper(impl->m_pointsManager.points());
		auto actor = impl->m_pointsManager.pointsActor();
		actor->SetMapper(mapper);
		mapper->Delete();

		actor->GetProperty()->SetPointSize(impl->m_displaySetting.tinPointSize * v->devicePixelRatioF());
		actor->GetProperty()->SetOpacity(impl->m_displaySetting.tinOpacity);
		actorCollection()->AddItem(actor);

		impl->m_pointsManager.setSelectedPointsSize((impl->m_displaySetting.tinPointSize + 2) * v->devicePixelRatioF());
	} else {
		auto mapper = cs->buildPointDataMapper(impl->m_tinManager.tin());
		auto actor = impl->m_tinManager.tinActor();
		actor->SetMapper(mapper);
		mapper->Delete();

		actor->GetProperty()->SetPointSize(impl->m_displaySetting.tinPointSize * v->devicePixelRatioF());
		actor->GetProperty()->SetLineWidth(impl->m_displaySetting.tinLineWidth * v->devicePixelRatioF());
		actor->GetProperty()->SetOpacity(impl->m_displaySetting.tinOpacity);
		actorCollection()->AddItem(actor);

		switch (impl->m_displaySetting.tinRepresentation.value()) {
		case DisplaySetting::TinRepresentation::Points:
			actor->GetProperty()->SetRepresentationToPoints();
			break;
		case DisplaySetting::TinRepresentation::Wireframe:
			actor->GetProperty()->SetRepresentationToWireframe();
			break;
		case DisplaySetting::TinRepresentation::Surface:
			actor->GetProperty()->SetRepresentationToSurface();
			break;
		}
	}

	impl->m_tinManager.updateBreakLinesActorSettings();
	impl->m_polygonsManager.updatePolygonsActorSettings();

	updateVisibilityWithoutRendering();
}

void GeoDataPointmap::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void GeoDataPointmap::openDisplaySettingDialog()
{
	dynamic_cast<PreProcessorGeoDataDataItemI*>(parent())->showPropertyDialog();
}

void GeoDataPointmap::openMappingSettingDialog()
{
	MappingSettingDialog dialog(preProcessorWindow());
	dialog.setSetting(impl->m_mappingSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	impl->m_mappingSetting = dialog.setting();

	iRICUndoStack::instance().clear();
}

void GeoDataPointmap::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{
	auto controller = impl->m_polygonsManager.polygonsColorMap()->legendSetting()->imgSetting()->controller();
	controller->handleMousePressEvent(event, v);
	if (controller->mouseEventMode() == ImageSettingContainer::Controller::MouseEventMode::Normal) {
		impl->m_activeController->handleMousePressEvent(event, v);
	} else {
		std::vector<ImageSettingContainer::Controller*> controllers;
		controllers.push_back(controller);
		ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
	}

	if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = event->pos();
	}
}
void GeoDataPointmap::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{

	auto controller = impl->m_polygonsManager.polygonsColorMap()->legendSetting()->imgSetting()->controller();
	controller->handleMouseReleaseEvent(event, v);
	if (controller->mouseEventMode() == ImageSettingContainer::Controller::MouseEventMode::Normal) {
		impl->m_activeController->handleMouseReleaseEvent(event, v);
	} else {
		std::vector<ImageSettingContainer::Controller*> controllers;
		controllers.push_back(controller);
		ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
	}

	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

QStringList GeoDataPointmap::containedFiles() const
{
	QStringList ret;
	ret << relativeFilename();
	ret << relativeFilename().replace(".dat", "_polygons.dat");

	return ret;
}

void GeoDataPointmap::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{
	auto controller = impl->m_polygonsManager.polygonsColorMap()->legendSetting()->imgSetting()->controller();
	controller->handleMouseMoveEvent(event, v, true);
	if (controller->mouseEventMode() == ImageSettingContainer::Controller::MouseEventMode::Normal) {
		impl->m_activeController->handleMouseMoveEvent(event, v);
	} else {
		std::vector<ImageSettingContainer::Controller*> controllers;
		controllers.push_back(controller);
		ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
	}
}

void GeoDataPointmap::mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{
	impl->m_activeController->handleMouseDoubleClickEvent(event, v);
}

void GeoDataPointmap::informDeselection(PreProcessorGraphicsViewI* /*v*/)
{
	impl->m_pointsManager.clearSelection();
}

void GeoDataPointmap::handleResize(QResizeEvent* event, PreProcessorGraphicsViewI* v)
{
	auto controller = impl->m_polygonsManager.polygonsColorMap()->legendSetting()->imgSetting()->controller();
	controller->handleResize(event, v);
}

void GeoDataPointmap::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v)
{
	impl->m_activeController->handleKeyPressEvent(event, v);
}

void GeoDataPointmap::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v)
{
	impl->m_activeController->handleKeyReleaseEvent(event, v);
}

void GeoDataPointmap::remeshTin(bool nodialog)
{
	graphicsView()->setCursor(Qt::WaitCursor);

	bool ok = rebuildTinFromPoints(!nodialog);
	if (! ok) {return;}

	graphicsView()->setCursor(Qt::ArrowCursor);

	updateActorSetting();
	renderGraphicsView();
	iRICUndoStack::instance().clear();
	if (! nodialog) {
		QMessageBox::information(preProcessorWindow(), tr("Information"), tr("Remeshing TINs finished."));
	}
}

void GeoDataPointmap::removeTrianglesWithLongEdgeStart()
{
	/*
	if (m_representation == GeoDataPointmapRepresentationDialog::Points) {
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you want to remove triangles with long edges, you have to switch Display Method to Wireframe or Surface. If you press OK button, Display Method is switched to Wireframe"), QMessageBox::Ok | QMessageBox::Cancel);
		if (result == QMessageBox::Cancel) {return;}

		if (m_needRemeshing) {
			remeshTINS();
		}
		m_representation = GeoDataPointmapRepresentationDialog::Wireframe;
		updateActorSettings();
	}
	m_longEdgeRemover = new TrianglesWithLongEdgeRemover(this);
	*/
}

void GeoDataPointmap::removeTrianglesWithLongEdgeEnd()
{
	/*
	delete m_longEdgeRemover;
	m_longEdgeRemover = nullptr;
	*/
}

void GeoDataPointmap::mergePointmaps()
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemI*> (parent()->parent());
	std::vector<PreProcessorGeoDataDataItemI*> dataToMerge;

	for (auto item : gItem->childItems()) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemI*> (item);
		auto geoData = p->geoData();
		if (geoData == this) {continue;}

		if (dynamic_cast<GeoDataPointmap*> (geoData) != nullptr) {
			dataToMerge.push_back(p);
		}
	}
	if (dataToMerge.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("There is no other data to merge in \"%1\" group.").arg(gItem->condition()->caption()));
		return;
	}

	GeoDataPointmapMergeSettingDialog dialog(preProcessorWindow());
	dialog.setItems(dataToMerge);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	dataToMerge = dialog.selectedItems();

	std::set<QPointF, QPointFCompare> pointSet;

	// insert points in this data to pointSet first
	auto myPoints = impl->m_pointsManager.points()->GetPoints();
	auto myValues = vtkDoubleArray::SafeDownCast(impl->m_pointsManager.points()->GetPointData()->GetArray(VALUES));
	auto myVerts = impl->m_pointsManager.points()->GetVerts();

	double v[3];
	for (vtkIdType i = 0; i < myPoints->GetNumberOfPoints(); ++i) {
		myPoints->GetPoint(i, v);
		pointSet.insert(QPointF(v[0], v[1]));
	}

	auto duplicatePoints = 0;
	for (auto item : dataToMerge) {
		auto geoData = item->geoData();
		auto pointMap = dynamic_cast<GeoDataPointmap*> (geoData);

		auto points = pointMap->impl->m_pointsManager.points()->GetPoints();
		auto vals = vtkDoubleArray::SafeDownCast(pointMap->impl->m_pointsManager.points()->GetPointData()->GetArray(VALUES));

		double val;
		vtkIdType vId;
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, v);
			val = vals->GetValue(i);
			QPointF p(v[0], v[1]);
			auto it = pointSet.find(p);
			if (it != pointSet.end()) {
				++ duplicatePoints;
				continue;
			}
			vId = myPoints->GetNumberOfPoints();

			myPoints->InsertNextPoint(v);
			myValues->InsertNextValue(val);
			myVerts->InsertNextCell(1, &vId);
			pointSet.insert(p);
		}

		item->setDeleteSilently(true);
		delete item;
	}
	impl->m_pointsManager.points()->Modified();
	myPoints->Modified();
	myValues->Modified();
	myVerts->Modified();

	rebuildTinFromPoints(false);

	updateActorSetting();
	renderGraphicsView();

	if (duplicatePoints > 0) {
		QMessageBox::information(preProcessorWindow(), tr("Information"), tr("%1 duplicate points were found in merge targets, and omitted.").arg(duplicatePoints));
	}

	// merge operation is not undoable.
	iRICUndoStack::instance().clear();
}

void GeoDataPointmap::togglePointsEditMode()
{
	impl->m_pointEditModeAction->setChecked(true);
	impl->m_tinEditModeAction->setChecked(false);
	impl->m_polygonsEditModeAction->setChecked(false);

	auto v = dataModel()->graphicsView();
	impl->m_activeController->deactivate(v);
	impl->m_activeController = &impl->m_pointsManager;
	impl->m_activeController->activate(v);

	updateMenu();
	updateActorSetting();
	renderGraphicsView();
}

void GeoDataPointmap::toggleTinEditMode()
{
	impl->m_tinEditModeAction->setChecked(true);
	impl->m_pointEditModeAction->setChecked(false);
	impl->m_polygonsEditModeAction->setChecked(false);


	if (impl->m_activeController == &impl->m_pointsManager) {
		rebuildTinFromPointsIfNeeded();
	}
	auto v = dataModel()->graphicsView();
	impl->m_activeController->deactivate(v);
	impl->m_activeController = &impl->m_tinManager;
	impl->m_activeController->activate(v);

	updateMenu();
	updateActorSetting();
	renderGraphicsView();
}

void GeoDataPointmap::togglePolyonsEditMode()
{
	impl->m_polygonsEditModeAction->setChecked(true);
	impl->m_pointEditModeAction->setChecked(false);
	impl->m_tinEditModeAction->setChecked(false);

	if (impl->m_activeController == &impl->m_pointsManager) {
		rebuildTinFromPointsIfNeeded();
	}

	auto v = dataModel()->graphicsView();
	impl->m_activeController->deactivate(v);
	impl->m_activeController = &impl->m_polygonsManager;
	impl->m_activeController->activate(v);

	updateMenu();
	updateActorSetting();
	renderGraphicsView();
}

QVector<vtkIdType> GeoDataPointmap::selectedVertices()
{
	QSet<vtkIdType> tmpsel;
	QVector<vtkIdType> selected;
	vtkIdType numPoints;
	vtkIdType* points;
	auto selectedVerts = impl->m_pointsManager.selectedPoints()->GetVerts();
	selectedVerts->InitTraversal();
	for (vtkIdType cellId = 0; cellId < selectedVerts->GetNumberOfCells(); ++cellId) {
		selectedVerts->GetNextCell(numPoints, points);
		for (int ptid = 0; ptid < numPoints; ++ptid) {
			tmpsel.insert(*(points + ptid));
		}
	}
	for (auto it = tmpsel.begin(); it != tmpsel.end(); ++it) {
		selected.push_back(*it);
	}
	qSort(selected);
	return selected;
}

iRICLib::H5CgnsGeographicDataGroup::Type GeoDataPointmap::iRICLibType() const
{
	return iRICLib::H5CgnsGeographicDataGroup::Type::Pointmap;
}

void GeoDataPointmap::pushModifyCommand(QUndoCommand* command)
{
	pushRenderCommand(new ModifyCommand(command, this));
}

vtkCell* GeoDataPointmap::findTinCell(double x, double y, double *weights)
{
	return impl->m_tinManager.findCell(x, y, weights);
}

GeoDataProxy* GeoDataPointmap::getProxy()
{
	return new GeoDataPointmapProxy(this);
}

void GeoDataPointmap::doApplyOffset(double x, double y)
{
	auto points1 = impl->m_pointsManager.points()->GetPoints();
	auto points2 = impl->m_tinManager.tin()->GetPoints();

	applyOffsetToPoints(points1, x, y);
	if (points2 != points1) {
		applyOffsetToPoints(points2, x, y);
	}
}
