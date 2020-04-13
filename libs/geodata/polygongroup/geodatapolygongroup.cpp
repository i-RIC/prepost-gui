#include "geodatapolygongroup.h"
#include "geodatapolygongroupattributebrowser.h"
#include "geodatapolygongroupcolorsettingdialog.h"
#include "geodatapolygongroupmergesettingdialog.h"
#include "geodatapolygongrouppolygon.h"
#include "private/geodatapolygongroup_editpropertycommand.h"
#include "private/geodatapolygongroup_impl.h"
#include "private/geodatapolygongroup_sortcommand.h"

#include <geodata/polygon/geodatapolygon.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/gridcond/base/gridattributeeditnameandvaluedialog.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/zdepthrange.h>

#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointF>

#include <vtkActorCollection.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

#include <geos/geom/Envelope.h>
#include <geos/geom/Polygon.h>
#include <geos/index/quadtree/Quadtree.h>

#include <string>

namespace {

std::string VALUE = "value";

int indexOfFirstSelected(const std::vector<GeoDataPolygonGroupPolygon*>& pols, const std::unordered_set<GeoDataPolygonGroupPolygon*>& selectedPols)
{
	for (auto it = pols.begin(); it != pols.end(); ++it) {
		if (selectedPols.find(*it) != selectedPols.end()) {
			return (it - pols.begin());
		}
	}
	return -1;
}

std::vector<GeoDataPolygonGroupPolygon*> getSelectedPolysVector(const std::vector<GeoDataPolygonGroupPolygon*>& pols, const std::unordered_set<GeoDataPolygonGroupPolygon*>& selectedPols)
{
	std::vector<GeoDataPolygonGroupPolygon*> ret;
	for (auto p : pols) {
		if (selectedPols.find(p) == selectedPols.end()) {continue;}

		ret.push_back(p);
	}
	return ret;
}

std::vector<GeoDataPolygonGroupPolygon*> getNotSelectedPolysVector(const std::vector<GeoDataPolygonGroupPolygon*>& pols, const std::unordered_set<GeoDataPolygonGroupPolygon*>& selectedPols)
{
	std::vector<GeoDataPolygonGroupPolygon*> ret;
	for (auto p : pols) {
		if (selectedPols.find(p) != selectedPols.end()) {continue;}

		ret.push_back(p);
	}
	return ret;
}

} // namespace

GeoDataPolygonGroup::GeoDataPolygonGroup(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoData(d, creator, condition),
	impl {new Impl {this}}
{
	ScalarsToColorsContainer* stcc = scalarsToColorsContainer();
	if (stcc != nullptr) {
		auto mapper = impl->m_paintActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_edgesActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPolygonsEdgesActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);

		mapper = impl->m_selectedPolygonsPointsActor->GetMapper();
		mapper->SetLookupTable(stcc->vtkDarkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}

	actorCollection()->AddItem(impl->m_paintActor);
	actorCollection()->AddItem(impl->m_edgesActor);

	renderer()->AddActor(impl->m_paintActor);
	renderer()->AddActor(impl->m_edgesActor);
	renderer()->AddActor(impl->m_selectedPolygonsEdgesActor);
	renderer()->AddActor(impl->m_selectedPolygonsPointsActor);

	makeConnections();

	impl->m_attributeBrowser->setPolygonGroup(this);
	preProcessorWindow()->addDockWidget(Qt::LeftDockWidgetArea, impl->m_attributeBrowser);
	impl->m_attributeBrowser->hide();

	if (gridAttribute() && gridAttribute()->isReferenceInformation()) {
		impl->m_colorSetting.mapping = GeoDataPolygonGroupColorSettingDialog::Arbitrary;
	}

	impl->updateActorSetting();
}

GeoDataPolygonGroup::~GeoDataPolygonGroup()
{
	actorCollection()->RemoveAllItems();
	renderer()->RemoveActor(impl->m_paintActor);
	renderer()->RemoveActor(impl->m_edgesActor);
	renderer()->RemoveActor(impl->m_selectedPolygonsEdgesActor);
	renderer()->RemoveActor(impl->m_selectedPolygonsPointsActor);

	delete impl;
}

void GeoDataPolygonGroup::addPolygon(GeoDataPolygonGroupPolygon* polygon)
{
	impl->m_polygons.push_back(polygon);
}

std::vector<GeoDataPolygonGroupPolygon*> GeoDataPolygonGroup::polygonsInBoundingBox(double xmin, double xmax, double ymin, double ymax) const
{
	auto env = new geos::geom::Envelope(xmin, xmax, ymin, ymax);
	std::vector<void*> qret;
	impl->m_qTree->query(env, qret);
	delete env;

	std::vector<GeoDataPolygonGroupPolygon*> ret;
	for (void* vptr: qret) {
		auto geom = reinterpret_cast<GeoDataPolygonGroupPolygon*> (vptr);
		if (geom == nullptr) {continue;}
		double gxmin, gxmax, gymin, gymax;
		geom->getBoundingRect(&gxmin, &gxmax, &gymin, &gymax);
		if (gxmax < xmin) {continue;}
		if (gxmin > xmax) {continue;}
		if (gymax < ymin) {continue;}
		if (gymin > ymax) {continue;}

		ret.push_back(geom);
	}
	return ret;
}

std::vector<GeoDataPolygonGroupPolygon*> GeoDataPolygonGroup::allPolygons() const
{
	return impl->m_polygons;
}

GeoDataPolygon* GeoDataPolygonGroup::editTargetPolygon() const
{
	return impl->m_editTargetPolygon;
}

int GeoDataPolygonGroup::editTargetPolygonIndex() const
{
	return impl->m_editTargetPolygonIndex;
}

bool GeoDataPolygonGroup::isSelected(GeoDataPolygonGroupPolygon* polygon)
{
	const auto& pols = impl->m_selectedPolygons;
	return (pols.find(polygon) != pols.end());
}

void GeoDataPolygonGroup::panTo(int row)
{
	double xmin, xmax, ymin, ymax;
	if (row < impl->m_editTargetPolygonIndex) {
		auto pol = impl->m_polygons.at(row);
		pol->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
	} else if (row == impl->m_editTargetPolygonIndex && impl->m_editTargetPolygon != nullptr) {
		auto pol = impl->m_editTargetPolygon->getGeosPolygon(QPointF(0, 0));
		auto env = pol->getEnvelopeInternal();
		xmin = env->getMinX();
		xmax = env->getMaxX();
		ymin = env->getMinY();
		ymax = env->getMaxY();
		delete pol;
	} else {
		int offset = 0;
		if (impl->m_editTargetPolygon != nullptr) {offset = -1;}
		auto pol = impl->m_polygons.at(row + offset);
		pol->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
	}
	auto v = graphicsView();
	v->panTo((xmin + xmax) * 0.5, (ymin + ymax) * 0.5);
}

void GeoDataPolygonGroup::updateVtkObjects()
{
	impl->m_points->Initialize();
	impl->m_paintPolyData->Initialize();
	impl->m_edgesPolyData->Initialize();

	auto edges = vtkSmartPointer<vtkCellArray>::New();
	auto tris = vtkSmartPointer<vtkCellArray>::New();

	auto edgeValues = vtkSmartPointer<vtkDoubleArray>::New();
	edgeValues->SetName(VALUE.c_str());
	auto triValues = vtkSmartPointer<vtkDoubleArray>::New();
	triValues->SetName(VALUE.c_str());

	vtkIdType offset = 0;
	for (auto pol : impl->m_polygons) {
		double v = pol->value().toDouble();
		for (const QPointF& p : pol->points()) {
			impl->m_points->InsertNextPoint(p.x(), p.y(), 0);
		}
		auto lineEdges = pol->lineEdges();
		vtkIdType pts[3];
		for (int i = 0; i < lineEdges.size() / 2; ++i) {
			pts[0] = lineEdges.at(i * 2) + offset;
			pts[1] = lineEdges.at(i * 2 + 1) + offset;
			edges->InsertNextCell(2, pts);
			edgeValues->InsertNextValue(v);
		}
		auto cells = pol->triangleCells();
		for (int i = 0; i < cells.size() / 3; ++i) {
			pts[0] = cells.at(i * 3) + offset;
			pts[1] = cells.at(i * 3 + 1) + offset;
			pts[2] = cells.at(i * 3 + 2) + offset;
			tris->InsertNextCell(3, pts);
			triValues->InsertNextValue(v);
		}
		pol->setIndexOffset(offset);
		offset = impl->m_points->GetNumberOfPoints();
	}
	impl->m_paintPolyData->SetPoints(impl->m_points);
	impl->m_paintPolyData->SetPolys(tris);
	impl->m_paintPolyData->GetCellData()->AddArray(triValues);
	impl->m_paintPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());

	impl->m_edgesPolyData->SetPoints(impl->m_points);
	impl->m_edgesPolyData->SetLines(edges);
	impl->m_edgesPolyData->GetCellData()->AddArray(edgeValues);
	impl->m_edgesPolyData->GetCellData()->SetActiveScalars(VALUE.c_str());
}

void GeoDataPolygonGroup::updateIndex()
{
	delete impl->m_qTree;
	impl->m_qTree = new geos::index::quadtree::Quadtree();
	for (GeoDataPolygonGroupPolygon* p : impl->m_polygons) {
		double xmin, xmax, ymin, ymax;
		p->getBoundingRect(&xmin, &xmax, &ymin, &ymax);
		auto env = new geos::geom::Envelope(xmin, xmax, ymin, ymax);
		impl->m_qTree->insert(env, p);
	}
}

void GeoDataPolygonGroup::setupMenu()
{
	m_menu->setTitle(tr("P&olygon Group"));

	updateMenu();
}

bool GeoDataPolygonGroup::addToolBarButtons(QToolBar* parent)
{
	return false;
}

void GeoDataPolygonGroup::showInitialDialog()
{
	// when added, add a new polygon, and show initial dialog
	addPolygon();
	impl->m_editTargetPolygon->showInitialDialog();
}

void GeoDataPolygonGroup::informSelection(PreProcessorGraphicsViewInterface* v)
{
	auto col = actorCollection();
	col->AddItem(impl->m_selectedPolygonsEdgesActor);
	col->AddItem(impl->m_selectedPolygonsPointsActor);

	if (impl->m_attributeBrowserIsShown) {
		impl->m_attributeBrowser->show();
	}

	updateVisibilityWithoutRendering();
}

void GeoDataPolygonGroup::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	mergeEditTargetPolygon();

	auto col = actorCollection();
	col->RemoveItem(impl->m_selectedPolygonsEdgesActor);
	col->RemoveItem(impl->m_selectedPolygonsPointsActor);

	impl->m_selectedPolygonsEdgesActor->VisibilityOff();
	impl->m_selectedPolygonsPointsActor->VisibilityOff();

	impl->m_attributeBrowser->blockSignals(true);
	impl->m_attributeBrowser->hide();
	impl->m_attributeBrowser->blockSignals(false);
}

void GeoDataPolygonGroup::addCustomMenuItems(QMenu* menu)
{}

void GeoDataPolygonGroup::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{}

void GeoDataPolygonGroup::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode != Impl::EditPolygon) {return;}

	impl->m_editTargetPolygon->keyPressEvent(event, v);
}

void GeoDataPolygonGroup::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode != Impl::EditPolygon) {return;}

	impl->m_editTargetPolygon->keyReleaseEvent(event, v);
}

void GeoDataPolygonGroup::mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode != Impl::EditPolygon) {return;}

	impl->m_editTargetPolygon->mouseDoubleClickEvent(event, v);
}

void GeoDataPolygonGroup::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::EditPolygon) {
		impl->m_editTargetPolygon->mouseMoveEvent(event, v);
	} else {
		auto box = dataModel()->mouseBoundingBox();
		box->setEndPoint(event->x(), event->y());

		renderGraphicsView();
	}
}

void GeoDataPolygonGroup::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::EditPolygon && event->button() == Qt::LeftButton) {
		impl->m_editTargetPolygon->mousePressEvent(event, v);
		if (
				impl->m_editTargetPolygon->mouseEventMode() == GeoDataPolygon::meNormal &&
				impl->m_editTargetPolygon->selectMode() == GeoDataPolygon::smNone)
		{
			mergeEditTargetPolygon();
		}
	}

	if (event->button() == Qt::LeftButton && impl->m_mode == Impl::Mode::Normal) {
		auto box = dataModel()->mouseBoundingBox();
		box->setStartPoint(event->x(), event->y());
		box->enable();

		v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	} else if (event->button() == Qt::RightButton) {
		impl->m_dragStartPoint = event->pos();
	}
}

void GeoDataPolygonGroup::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (impl->m_mode == Impl::EditPolygon) {
		if (event->button() == Qt::LeftButton) {
			impl->m_editTargetPolygon->mouseReleaseEvent(event, v);
		} else if (event->button() == Qt::RightButton) {
			if (! iRIC::isNear(impl->m_dragStartPoint, event->pos())) {return;}

			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	} else {
		if (event->button() == Qt::LeftButton) {
			auto box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());
			box->disable();

			if (iRIC::isNear(box->startPoint(), box->endPoint())) {
				double x = box->endPoint().x();
				double y = box->endPoint().y();
				v->viewportToWorld(x, y);
				impl->selectPolygonsIncludePoint(QPointF(x, y));
			} else {
				auto vtkPol = box->getPolygon();
				auto points = vtkPol->GetPoints();
				QPolygonF qpol;
				for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
					double p[3];
					points->GetPoint(i, p);
					qpol.push_back(QPointF(p[0], p[1]));
				}
				qpol.push_back(qpol.at(0));
				impl->selectPolygonsIncludedInPolygon(qpol);
			}

			v->restoreUpdateRate();
			if (impl->m_selectedPolygons.size() == 1) {
				impl->setupEditTargetPolygonFromSelectedPolygon();
			}
			impl->updateAttributeBrowser();
			renderGraphicsView();
		} else {
			if (! iRIC::isNear(impl->m_dragStartPoint, event->pos())) {return;}

			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GeoDataPolygonGroup::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GeoDataPolygonGroup::assignActorZValues(const ZDepthRange& range)
{
	impl->m_depthRange = range;
	impl->m_edgesActor->SetPosition(0, 0, range.max());
	impl->m_paintActor->SetPosition(0, 0, range.min());
	impl->m_selectedPolygonsEdgesActor->SetPosition(0, 0, range.max());
	impl->m_selectedPolygonsPointsActor->SetPosition(0, 0, range.max());
}

QDialog* GeoDataPolygonGroup::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolygonGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_colorSetting);
	auto gridAtt = gridAttribute();
	if (gridAtt != nullptr) {
		dialog->setIsReferenceInformation(gridAtt->isReferenceInformation());
	}
	return dialog;
}

void GeoDataPolygonGroup::handlePropertyDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GeoDataPolygonGroupColorSettingDialog*> (d);
	pushRenderCommand(new EditPropertyCommand(dialog->setting(), this));
}

bool GeoDataPolygonGroup::getValueRange(double* min, double* max)
{
	if (impl->m_polygons.size() == 0 && impl->m_editTargetPolygon == nullptr) {return false;}
	bool first = true;
	for (GeoDataPolygonGroupPolygon* p : impl->m_polygons) {
		double v = p->value().toDouble();
		if (first || v < *min) {*min = v;}
		if (first || v > *max) {*max = v;}
		first = false;
	}
	if (impl->m_editTargetPolygon != nullptr) {
		double v = impl->m_editTargetPolygon->variantValue().toDouble();
		if (first || v < *min) {*min = v;}
		if (first || v > *max) {*max = v;}
	}
	return true;
}

void GeoDataPolygonGroup::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPolygonGroup::addPolygon()
{
	if (impl->m_editTargetPolygon != nullptr) {
		mergeEditTargetPolygon();
	}
	auto p = new GeoDataPolygon(parent(), creator(), gridAttribute());;
	impl->m_editTargetPolygon = p;
	auto caption = tr("Polygon%1").arg(impl->m_polygons.size() + 1);
	p->setCaption(caption);
	p->assignActorZValues(impl->m_depthRange);
	p->informSelection(graphicsView());
	p->updateActionStatus();
	connect(p, SIGNAL(nameAndValueEdited()), this, SLOT(updateAttributeBrowser()));

	impl->m_selectedPolygons.clear();
	impl->updateSelectedPolygonsVtkObjects();
	impl->updateAttributeBrowser();
	renderGraphicsView();

	impl->m_mode = Impl::EditPolygon;
	updateMenu();
}

void GeoDataPolygonGroup::editName()
{
	bool ok;
	QString newName = QInputDialog::getText(preProcessorWindow(), tr("Edit name"), tr("Name:"), QLineEdit::Normal, "", &ok);
	if (! ok) {return;}
	if (newName.isEmpty()) {return;}

	for (auto p : impl->m_selectedPolygons) {
		p->setName(newName);
	}
	impl->updateAttributeBrowser();
}

void GeoDataPolygonGroup::editNameAndValue()
{
	auto dialog = m_gridAttribute->editNameAndValueDialog(preProcessorWindow());
	auto i = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(parent()->parent());
	dialog->setWindowTitle(QString(tr("Edit %1 value")).arg(i->condition()->caption()));
	dialog->setName(caption());
	i->setupEditWidget(dialog->widget());
	dialog->setName("");
	dialog->clearValue();
	int ret = dialog->exec();
	if (ret == QDialog::Rejected) {
		delete dialog;
		return;
	}
	QString newName = dialog->name();
	bool valSelected = dialog->isValueSelected();
	QVariant newValue = dialog->variantValue();

	for (auto p : impl->m_selectedPolygons) {
		if (! newName.isEmpty()) {
			p->setName(newName);
		}
		if (valSelected) {
			p->setValue(newValue);
		}
	}
	delete dialog;

	updateVtkObjects();
	impl->updateSelectedPolygonsVtkObjects();
	impl->updateAttributeBrowser();
	auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (parent());
	p->informValueRangeChange();
	p->informDataChange();
}

void GeoDataPolygonGroup::mergePolygonsAndPolygonGroups()
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent());
	std::vector<PreProcessorGeoDataDataItemInterface*> dataToMerge;
	for (auto item : gItem->childItems()) {
		PreProcessorGeoDataDataItemInterface* p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (item);
		auto geoData = p->geoData();
		if (geoData == this) {continue;}

		auto pol = dynamic_cast<GeoDataPolygon*> (geoData);
		auto polg = dynamic_cast<GeoDataPolygonGroup*> (geoData);

		if (pol != nullptr || polg != nullptr) {
			dataToMerge.push_back(p);
		}
	}
	if (dataToMerge.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("There is no other polygon or polygon group in \"%1\" group.").arg(gItem->condition()->caption()));
		return;
	}

	GeoDataPolygonGroupMergeSettingDialog dialog(preProcessorWindow());
	dialog.setItems(dataToMerge);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	dataToMerge = dialog.selectedItems();

	for (auto item : dataToMerge) {
		auto geoData = item->geoData();
		auto pol = dynamic_cast<GeoDataPolygon*> (geoData);
		auto polg = dynamic_cast<GeoDataPolygonGroup*> (geoData);
		if (pol != nullptr) {
			impl->mergeToThis(pol);
		} else if (polg != nullptr) {
			impl->mergeToThis(polg);
		}
		delete item;
	}
	updateVtkObjects();
	impl->updateSelectedPolygonsVtkObjects();
	impl->updateAttributeBrowser();
}

void GeoDataPolygonGroup::moveSelectedPolygonsToTop()
{
	mergeEditTargetPolygon();
	auto selected = getSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	auto notSelected = getNotSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	notSelected.insert(notSelected.begin(), selected.begin(), selected.end());

	pushCommand(new SortCommand(notSelected, this));
}

void GeoDataPolygonGroup::moveSelectedPolygonsToBottom()
{
	mergeEditTargetPolygon();
	auto selected = getSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	auto notSelected = getNotSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	notSelected.insert(notSelected.end(), selected.begin(), selected.end());

	pushCommand(new SortCommand(notSelected, this));
}

void GeoDataPolygonGroup::moveSelectedPolygonsUp()
{
	mergeEditTargetPolygon();
	auto index = indexOfFirstSelected(impl->m_polygons, impl->m_selectedPolygons);
	if (index == 0) {return;}

	auto selected = getSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	auto notSelected = getNotSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	notSelected.insert(notSelected.begin() + index - 1, selected.begin(), selected.end());

	pushCommand(new SortCommand(notSelected, this));
}

void GeoDataPolygonGroup::moveSelectedPolygonsDown()
{
	mergeEditTargetPolygon();
	auto index = indexOfFirstSelected(impl->m_polygons, impl->m_selectedPolygons);
	if (index == impl->m_polygons.size() - impl->m_selectedPolygons.size()) {return;}

	auto selected = getSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	auto notSelected = getNotSelectedPolysVector(impl->m_polygons, impl->m_selectedPolygons);
	notSelected.insert(notSelected.begin() + index + 1, selected.begin(), selected.end());

	pushCommand(new SortCommand(notSelected, this));
}

void GeoDataPolygonGroup::deleteSelectedPolygons()
{
	if (QMessageBox::No == QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to delete selected polygons?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
		return;
	}
	std::vector<GeoDataPolygonGroupPolygon*> newPolygons;

	for (auto p : impl->m_polygons) {
		if (impl->m_selectedPolygons.find(p) != impl->m_selectedPolygons.end()) {
			delete p;
		} else {
			newPolygons.push_back(p);
		}
	}
	impl->m_polygons = newPolygons;
	impl->m_selectedPolygons.clear();

	if (impl->m_editTargetPolygon != nullptr) {
		delete impl->m_editTargetPolygon;
		impl->m_editTargetPolygon = nullptr;
		impl->m_mode = Impl::Normal;
	}

	// this operation is not undoable.
	iRICUndoStack::instance().clear();

	updateVtkObjects();
	updateIndex();
	impl->updateSelectedPolygonsVtkObjects();
	impl->updateAttributeBrowser();
	impl->updateActionStatus();
}

void GeoDataPolygonGroup::editColorSetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPolygonGroup::showAttributeBrowser()
{
	impl->m_attributeBrowser->show();
	impl->updateAttributeBrowser();
}

void GeoDataPolygonGroup::updateAttributeBrowser()
{
	impl->updateAttributeBrowser();
}

void GeoDataPolygonGroup::handleAttributeBrowserVisibilityChange(bool visible)
{
	impl->m_attributeBrowserIsShown = visible;
}

GeoDataPolygonGroupColorSettingDialog::Setting GeoDataPolygonGroup::colorSetting() const
{
	return impl->m_colorSetting;
}

void GeoDataPolygonGroup::setColorSetting(const GeoDataPolygonGroupColorSettingDialog::Setting& setting)
{
	impl->m_colorSetting = setting;
	impl->updateActorSetting();
}

void GeoDataPolygonGroup::mergeEditTargetPolygon()
{
	if (impl->m_editTargetPolygon == nullptr) {return;}

	auto p = impl->mergeToThis(impl->m_editTargetPolygon, impl->m_editTargetPolygonIndex);

	delete impl->m_editTargetPolygon;
	impl->m_editTargetPolygon = nullptr;
	impl->m_editTargetPolygonIndex = 0;
	impl->m_mode = Impl::Normal;
	impl->m_selectedPolygons.insert(p);

	updateVtkObjects();
	updateIndex();
	updateMenu();
	impl->updateSelectedPolygonsVtkObjects();

	iRICUndoStack::instance().clear();
}

void GeoDataPolygonGroup::updateMenu()
{
	auto att = gridAttribute();
	bool isRef = att && att->isReferenceInformation();
	GeoDataPolygon* p = impl->m_dummyPolygonForMenu;
	if (impl->m_editTargetPolygon != nullptr) {
		p = impl->m_editTargetPolygon;
	}

	m_menu->clear();

	m_menu->addAction(m_editNameAction);

	m_menu->addSeparator();
	m_menu->addAction(impl->m_addAction);

	m_menu->addSeparator();
	if (impl->m_selectedPolygons.size() > 1) {
		if (isRef) {
			m_menu->addAction(impl->m_editNameAction);
		} else {
			m_menu->addAction(impl->m_editNameAndValueAction);
		}
	} else {
		if (isRef) {
			m_menu->addAction(p->editNameAction());
		} else {
			m_menu->addAction(p->editNameAndValueAction());
		}
	}

	m_menu->addSeparator();
	m_menu->addAction(p->addVertexAction());
	m_menu->addAction(p->removeVertexAction());
	m_menu->addAction(p->coordEditAction());

	m_menu->addSeparator();
	m_menu->addAction(p->holeModeAction());
	m_menu->addAction(p->deleteAction());

	m_menu->addSeparator();
	auto sortMenu = m_menu->addMenu(tr("&Sort"));
	sortMenu->addAction(impl->m_moveToTopAction);
	sortMenu->addAction(impl->m_moveUpAction);
	sortMenu->addAction(impl->m_moveDownAction);
	sortMenu->addAction(impl->m_moveToBottomAction);

	m_menu->addSeparator();
	m_menu->addAction(impl->m_mergeAction);
	m_menu->addAction(impl->m_editColorSettingAction);
	m_menu->addAction(impl->m_attributeBrowserAction);

	m_menu->addSeparator();
	m_menu->addAction(impl->m_deleteAction);

	impl->m_rightClickingMenu->clear();

	impl->m_rightClickingMenu->addAction(impl->m_addAction);

	impl->m_rightClickingMenu->addSeparator();
	if (impl->m_selectedPolygons.size() > 1) {
		if (isRef) {
			impl->m_rightClickingMenu->addAction(impl->m_editNameAction);
		} else {
			impl->m_rightClickingMenu->addAction(impl->m_editNameAndValueAction);
		}
	} else {
		if (isRef) {
			impl->m_rightClickingMenu->addAction(p->editNameAction());
		} else {
			impl->m_rightClickingMenu->addAction(p->editNameAndValueAction());
		}
	}

	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(p->addVertexAction());
	impl->m_rightClickingMenu->addAction(p->removeVertexAction());
	impl->m_rightClickingMenu->addAction(p->coordEditAction());

	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(p->holeModeAction());
	impl->m_rightClickingMenu->addAction(p->deleteAction());

	impl->m_rightClickingMenu->addSeparator();
	sortMenu = impl->m_rightClickingMenu->addMenu(tr("&Sort"));
	sortMenu->addAction(impl->m_moveToTopAction);
	sortMenu->addAction(impl->m_moveUpAction);
	sortMenu->addAction(impl->m_moveDownAction);
	sortMenu->addAction(impl->m_moveToBottomAction);

	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_mergeAction);
	impl->m_rightClickingMenu->addAction(impl->m_editColorSettingAction);
	impl->m_rightClickingMenu->addAction(impl->m_attributeBrowserAction);

	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_deleteAction);
}

void GeoDataPolygonGroup::makeConnections()
{
	connect(impl->m_addAction, SIGNAL(triggered()), this, SLOT(addPolygon()));
	connect(impl->m_selectAction, SIGNAL(triggered()), this, SLOT(selectPolygons()));
	connect(impl->m_mergeAction, SIGNAL(triggered()), this, SLOT(mergePolygonsAndPolygonGroups()));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedPolygons()));
	connect(impl->m_editNameAction, SIGNAL(triggered()), this, SLOT(editName()));
	connect(impl->m_editNameAndValueAction, SIGNAL(triggered()), this, SLOT(editNameAndValue()));
	connect(impl->m_editColorSettingAction, SIGNAL(triggered()), this, SLOT(editColorSetting()));
	connect(impl->m_attributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
	connect(impl->m_moveToTopAction, SIGNAL(triggered()), this, SLOT(moveSelectedPolygonsToTop()));
	connect(impl->m_moveToBottomAction, SIGNAL(triggered()), this, SLOT(moveSelectedPolygonsToBottom()));
	connect(impl->m_moveUpAction, SIGNAL(triggered()), this, SLOT(moveSelectedPolygonsUp()));
	connect(impl->m_moveDownAction, SIGNAL(triggered()), this, SLOT(moveSelectedPolygonsDown()));

	connect(impl->m_attributeBrowser, SIGNAL(visibilityChanged(bool)), this, SLOT(handleAttributeBrowserVisibilityChange(bool)));
}

void GeoDataPolygonGroup::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	impl->m_colorSetting.load(node);
}

void GeoDataPolygonGroup::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	impl->m_colorSetting.save(writer);
}

void GeoDataPolygonGroup::loadExternalData(const QString& filename)
{
	mergeEditTargetPolygon();
	impl->clear();

	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);

	int numPolygons;
	s >> numPolygons;

	for (int i = 0; i < numPolygons; ++i) {
		auto pol = new GeoDataPolygonGroupPolygon();
		pol->loadExternalData(&s);
		impl->m_polygons.push_back(pol);
	}
	f.close();

	updateVtkObjects();
	updateIndex();
	impl->m_attributeBrowser->update();
	impl->updateSelectedPolygonsVtkObjects();
	impl->updateActionStatus();
	impl->updateActorSetting();
}

void GeoDataPolygonGroup::saveExternalData(const QString& filename)
{
	mergeEditTargetPolygon();

	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);

	int numPolygons = impl->m_polygons.size();
	s << numPolygons;
	for (int i = 0; i < numPolygons; ++i) {
		GeoDataPolygonGroupPolygon* pol = impl->m_polygons.at(i);
		pol->saveExternalData(&s);
	}

	f.close();
}

void GeoDataPolygonGroup::doApplyOffset(double x, double y)
{
	for (GeoDataPolygonGroupPolygon* p : impl->m_polygons) {
		p->applyOffset(x, y);
	}
	updateVtkObjects();
	updateIndex();
	impl->updateSelectedPolygonsVtkObjects();
}
