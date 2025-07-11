#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridshapedataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <post/crosssection/postcrosssectioninternalwindow.h>
#include <post/crosssection/postcrosssectionwindow.h>
#include <misc/informationdialog.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

Post2dWindowGridShapeDataItem::Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_mode {Mode::Other},
	m_selectedpointId {-1},
	m_pointsActor {vtkActor::New()},
	m_pointsMapper {vtkDataSetMapper::New()},
	m_rightClickingMenu {new QMenu(postProcessorWindow())},
	m_openXsectionWindowAction {new QAction(tr("Open &Cross Section Window"), this)},
	m_openVXsectionWindowAction {new QAction(tr("Open &Longitudinal Cross Section Window"), this)},
	m_openEdgeXsectionWindowAction {new QAction(tr("Open &Cross Section Window"), this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();

	connect(m_openXsectionWindowAction, &QAction::triggered, this, &Post2dWindowGridShapeDataItem::openCrossSectionWindow);
	connect(m_openVXsectionWindowAction, &QAction::triggered, this, &Post2dWindowGridShapeDataItem::openVerticalCrossSectionWindow);
	connect(m_openEdgeXsectionWindowAction, &QAction::triggered, this, &Post2dWindowGridShapeDataItem::startEdgeSelect);

	auto g = gridDataItem()->grid();
	auto sg = dynamic_cast<v4Structured2dGrid*> (g);
	if (sg != nullptr) {
		m_mode = Mode::Structured2d;
		m_rightClickingMenu->addAction(m_openXsectionWindowAction);
		m_rightClickingMenu->addAction(m_openVXsectionWindowAction);
	}
	auto ug = dynamic_cast<v4Unstructured2dGrid*> (g);
	if (ug != nullptr) {
		auto firstCell = ug->vtkData()->data()->GetCell(0);
		if (firstCell->GetCellType() == VTK_LINE) {
			m_mode = Mode::Unstructured2dEdge;
			m_rightClickingMenu->addAction(m_openEdgeXsectionWindowAction);
			setupEdgeCounterParts();
		}
	}
}

Post2dWindowGridShapeDataItem::~Post2dWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_setting.outlineActor());
	r->RemoveActor(m_setting.wireframeActor());
	r->RemoveActor2D(m_setting.indexActor());

	r->RemoveActor(m_pointController.actor());
	r->RemoveActor(m_lineController.linesActor());
	r->RemoveActor(m_lineController.pointsActor());

	r->RemoveActor(m_pointsActor);

	m_pointsActor->Delete();
	m_pointsMapper->Delete();
}

void Post2dWindowGridShapeDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_setting.outlineActor());
	r->AddActor(m_setting.wireframeActor());
	r->AddActor2D(m_setting.indexActor());

	auto v = dataModel()->graphicsView();
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());

	r->AddActor(m_pointController.actor());
	m_pointController.actor()->GetProperty()->SetPointSize(5 * v->devicePixelRatioF());
	m_pointController.actor()->VisibilityOff();

	r->AddActor(m_lineController.linesActor());
	r->AddActor(m_lineController.pointsActor());
	m_lineController.linesActor()->GetProperty()->SetLineWidth(3 * v->devicePixelRatioF());
	m_lineController.pointsActor()->GetProperty()->SetPointSize(5 * v->devicePixelRatioF());
	m_lineController.linesActor()->VisibilityOff();
	m_lineController.pointsActor()->VisibilityOff();

	m_pointsActor->SetMapper(m_pointsMapper);
	m_pointsActor->GetProperty()->SetLighting(false);
	m_pointsActor->GetProperty()->SetRepresentationToPoints();
	m_pointsActor->GetProperty()->SetPointSize(3 * v->devicePixelRatioF());
	m_pointsActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
	m_pointsActor->VisibilityOff();
	r->AddActor(m_pointsActor);

	doUpdateActorSetting();
}

void Post2dWindowGridShapeDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowGridShapeDataItem::doUpdateActorSetting()
{
	auto grid2d = gridDataItem()->grid();
	if (grid2d == nullptr) {
		m_setting.update(m_actorCollection, m_actor2DCollection, nullptr, nullptr, nullptr, v4GridUtil::LABEL_NAME);
	} else {
		m_setting.update(m_actorCollection, m_actor2DCollection, grid2d->vtkData()->data(), grid2d->vtkFilteredData(), grid2d->vtkFilteredIndexData(), v4GridUtil::LABEL_NAME);
		m_pointsMapper->SetInputData(grid2d->vtkFilteredData());
	}
}

PostCrosssectionWindow* Post2dWindowGridShapeDataItem::createCrosssectionWindow(AbstractCrosssectionWindow::Mode mode) const
{
	auto w = iricMainWindow()->createPostCrosessionWindowWithNoSetting();
	auto iw = w->internalWindow();
	iw->setMode(mode);
	auto rItem = dynamic_cast<Post2dWindowCalculationResultDataItem*> (gridDataItem());
	auto zItem = rItem->zoneDataItem();
	iw->setDimensionAndZone(PostSolutionInfo::Dimension::dim2D, zItem->zoneName());

	return w;
}

Post2dWindowGridDataItemI* Post2dWindowGridShapeDataItem::gridDataItem() const
{
	return dynamic_cast<Post2dWindowGridDataItemI*> (parent());
}

void Post2dWindowGridShapeDataItem::openEdgeCrossSectionWindow()
{
	if (m_linePointIds.size() >= 2) {
		auto w = iricMainWindow()->createPostCrosessionWindowWithNoSetting();
		auto iw = w->internalWindow();
		iw->setMode(AbstractCrosssectionWindow::Mode::UnstructuredEdge);
		iw->setTargetLine(m_linePointIds);
		auto rItem = dynamic_cast<Post2dWindowCalculationResultDataItem*> (gridDataItem());
		auto zItem = rItem->zoneDataItem();
		iw->setDimensionAndZone(PostSolutionInfo::Dimension::dim2D, zItem->zoneName());
		iw->update();
		iw->updateEdgeFocus();
	}

	m_linePointIds.clear();
	m_linePoints.clear();
	m_lineController.setPolyLine(m_linePoints);
	m_lineController.pointsActor()->VisibilityOff();
	m_lineController.linesActor()->VisibilityOff();
	m_pointsActor->VisibilityOff();

	renderGraphicsView();
}

void Post2dWindowGridShapeDataItem::enterEdgeSelectMode()
{
	m_pointController.actor()->VisibilityOff();

	m_linePointIds.clear();
	m_linePoints.clear();
	m_lineController.setPolyLine(m_linePoints);
	m_lineController.pointsActor()->VisibilityOn();
	m_lineController.linesActor()->VisibilityOn();
	auto grid = gridDataItem()->grid();

	m_nextNodes.clear();
	for (vtkIdType i = 0; i < grid->nodeCount(); ++i) {
		m_nextNodes.insert(i);
	}
	m_pointsActor->VisibilityOn();

	renderGraphicsView();

	InformationDialog::information(postProcessorWindow(), tr("Information"), tr("Click on grid nodes to specify on which edge you want to visualize calculation result. Double click or press Enter key to finish selecting. You can cancel the operation, by pressing Escape key."), "post2d_edge_xsec_window");
}

void Post2dWindowGridShapeDataItem::leaveEdgeSelectMode()
{
	m_mode = Mode::Structured2d;

	m_linePointIds.clear();
	m_lineController.pointsActor()->VisibilityOff();
	m_lineController.linesActor()->VisibilityOff();
	m_pointsActor->VisibilityOff();

	renderGraphicsView();
}

void Post2dWindowGridShapeDataItem::setupEdgeCounterParts()
{
	m_edgeCounterParts.clear();

	auto g = gridDataItem()->grid();
	auto ug = dynamic_cast<v4Unstructured2dGrid*> (g);
	if (ug != nullptr) {
		auto grid = ug->vtkData()->data();
		for (vtkIdType i = 0; i < grid->GetNumberOfCells(); ++i) {
			auto cell = grid->GetCell(i);
			vtkIdType id0 = cell->GetPointId(0);
			vtkIdType id1 = cell->GetPointId(1);

			m_edgeCounterParts.insert({id0, id1});
			m_edgeCounterParts.insert({id1, id0});
		}
	}
}

void Post2dWindowGridShapeDataItem::openCrossSectionWindow()
{
	auto w = createCrosssectionWindow(AbstractCrosssectionWindow::Mode::StructuredIJ);
	auto iw = w->internalWindow();

	auto sg = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	vtkIdType i, j;
	sg->getPointIJIndex(m_selectedpointId, &i, &j);
	iw->setTarget(PostCrosssectionInternalWindow::Direction::I, i);
}

void Post2dWindowGridShapeDataItem::openVerticalCrossSectionWindow()
{
	auto w = createCrosssectionWindow(AbstractCrosssectionWindow::Mode::StructuredIJ);
	auto iw = w->internalWindow();

	auto sg = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	vtkIdType i, j;
	sg->getPointIJIndex(m_selectedpointId, &i, &j);
	iw->setTarget(PostCrosssectionInternalWindow::Direction::J, j);
}

void Post2dWindowGridShapeDataItem::startEdgeSelect()
{
	enterEdgeSelectMode();
}

void Post2dWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	doUpdateActorSetting();
}

void Post2dWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

void Post2dWindowGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Grid Shape Display Setting"));
	dialog->resize(dialog->sizeHint());

	return dialog;
}

void Post2dWindowGridShapeDataItem::informSelection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::selectedOutlineWidth * v->devicePixelRatioF());
	updateVisibility();
}

void Post2dWindowGridShapeDataItem::informDeselection(VTKGraphicsView* v)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(GridShapeSettingContainer::normalOutlineWidth * v->devicePixelRatioF());
}

void Post2dWindowGridShapeDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* /*v*/)
{
	if (m_mode != Mode::Unstructured2dEdge) {return;}

	if (event->key() == Qt::Key_Escape) {
		leaveEdgeSelectMode();
	}
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
		openEdgeCrossSectionWindow();
	}
}

void Post2dWindowGridShapeDataItem::mouseDoubleClickEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/)
{
	if (m_mode != Mode::Unstructured2dEdge) {return;}

	openEdgeCrossSectionWindow();
}

void Post2dWindowGridShapeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto view = dynamic_cast<Post2dWindowGraphicsView*> (v);
	auto pos = view->viewportToWorld(event->pos());

	auto grid = gridDataItem()->grid();
	auto pointId = grid->vtkData()->findClosestPointWithinRadius(v->stdDistance(iRIC::nearRadius()), pos.x(), pos.y(), 0);
	if (m_mode == Mode::Structured2d) {
		if (pointId < 0) {
			v->setCursor(Qt::ArrowCursor);
		} else {
			v->setCursor(Qt::CrossCursor);
		}
	} else if (m_mode == Mode::Unstructured2dEdge){
		bool pointSelected = false;
		if (pointId >= 0) {
			auto it = m_nextNodes.find(pointId);
			if (it != m_nextNodes.end()) {
				v->setCursor(Qt::CrossCursor);
				pointSelected = true;
			}
		}
		if (! pointSelected)  {
			v->setCursor(Qt::ArrowCursor);
		}
	}
}

void Post2dWindowGridShapeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* /*v*/)
{
	m_dragStartPoint = event->pos();
}

void Post2dWindowGridShapeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	if (event->button() == Qt::LeftButton) {
		auto view = dynamic_cast<Post2dWindowGraphicsView*> (v);
		auto pos = view->viewportToWorld(event->pos());

		auto grid = gridDataItem()->grid();
		if (m_mode == Mode::Structured2d) {
			m_selectedpointId = grid->vtkData()->findClosestPointWithinRadius(v->stdDistance(iRIC::nearRadius()), pos.x(), pos.y(), 0);
			if (m_selectedpointId < 0) {
				m_pointController.actor()->VisibilityOff();
				m_openXsectionWindowAction->setDisabled(true);
				m_openVXsectionWindowAction->setDisabled(true);
			} else {
				m_pointController.actor()->VisibilityOn();
				double v[3];
				grid->vtkData()->data()->GetPoint(m_selectedpointId, v);
				m_pointController.setPoint(QPointF(v[0], v[1]));
				m_openXsectionWindowAction->setEnabled(true);
				m_openVXsectionWindowAction->setEnabled(true);
			}
			renderGraphicsView();
		} else if (m_mode == Mode::Unstructured2dEdge) {
			auto pointId = grid->vtkData()->findClosestPointWithinRadius(v->stdDistance(iRIC::nearRadius()), pos.x(), pos.y(), 0);
			if (pointId >= 0) {
				auto it = m_nextNodes.find(pointId);
				if (it != m_nextNodes.end()) {
					m_linePointIds.push_back(pointId);
					double v[3];
					grid->vtkData()->data()->GetPoint(pointId, v);
					m_linePoints.push_back(QPointF(v[0], v[1]));
					m_lineController.setPolyLine(m_linePoints);

					m_nextNodes.clear();
					auto s = m_edgeCounterParts.lower_bound(pointId);
					auto e = m_edgeCounterParts.upper_bound(pointId);
					for (auto it2 = s; it2 != e; ++it2) {
						m_nextNodes.insert(it2->second);
					}
				}
			}
			renderGraphicsView();
		}
	} else if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void Post2dWindowGridShapeDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dWindowGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	double min = range.min();
	double mid = (range.min() + range.max()) / 2;
	double max = range.max();

	m_setting.outlineActor()->SetPosition(0, 0, min);
	m_setting.wireframeActor()->SetPosition(0, 0, min);

	m_pointsActor->SetPosition(0, 0, mid);

	m_pointController.actor()->SetPosition(0, 0, max);
	m_lineController.linesActor()->SetPosition(0, 0, max);
	m_lineController.pointsActor()->SetPosition(0, 0, max);
}
