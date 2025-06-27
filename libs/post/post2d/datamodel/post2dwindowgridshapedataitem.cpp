#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridshapedataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <post/crosssection/postcrosssectioninternalwindow.h>
#include <post/crosssection/postcrosssectionwindow.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>

Post2dWindowGridShapeDataItem::Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_selectedpointId {-1},
	m_rightClickingMenu {new QMenu(postProcessorWindow())},
	m_openXsectionWindowAction {new QAction(tr("Open &Cross Section Window"), this)},
	m_openVXsectionWindowAction {new QAction(tr("Open &Longitudinal Cross Section Window"), this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();

	connect(m_openXsectionWindowAction, &QAction::triggered, this, &Post2dWindowGridShapeDataItem::openCrossSectionWindow);
	connect(m_openVXsectionWindowAction, &QAction::triggered, this, &Post2dWindowGridShapeDataItem::openVerticalCrossSectionWindow);

	auto g = gridDataItem()->grid();
	auto sg = dynamic_cast<v4Structured2dGrid*> (g);
	if (sg != nullptr) {
		m_rightClickingMenu->addAction(m_openXsectionWindowAction);
		m_rightClickingMenu->addAction(m_openVXsectionWindowAction);
	}
}

Post2dWindowGridShapeDataItem::~Post2dWindowGridShapeDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_setting.outlineActor());
	r->RemoveActor(m_setting.wireframeActor());
	r->RemoveActor2D(m_setting.indexActor());
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
	}
}

PostCrosssectionWindow* Post2dWindowGridShapeDataItem::createCrosssectionWindow() const
{
	auto w = iricMainWindow()->createPostCrosessionWindowWithNoSetting();
	auto iw = w->internalWindow();
	auto rItem = dynamic_cast<Post2dWindowCalculationResultDataItem*> (gridDataItem());
	auto zItem = rItem->zoneDataItem();
	iw->setDimensionAndZone(PostSolutionInfo::Dimension::dim2D, zItem->zoneName());

	return w;
}

Post2dWindowGridDataItemI* Post2dWindowGridShapeDataItem::gridDataItem() const
{
	return dynamic_cast<Post2dWindowGridDataItemI*> (parent());
}

void Post2dWindowGridShapeDataItem::openCrossSectionWindow()
{
	auto w = createCrosssectionWindow();
	auto iw = w->internalWindow();

	auto sg = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	vtkIdType i, j;
	sg->getPointIJIndex(m_selectedpointId, &i, &j);
	iw->setTarget(PostCrosssectionInternalWindow::Direction::I, i);
}

void Post2dWindowGridShapeDataItem::openVerticalCrossSectionWindow()
{
	auto w = createCrosssectionWindow();
	auto iw = w->internalWindow();

	auto sg = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	vtkIdType i, j;
	sg->getPointIJIndex(m_selectedpointId, &i, &j);
	iw->setTarget(PostCrosssectionInternalWindow::Direction::J, j);
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

void Post2dWindowGridShapeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto view = dynamic_cast<Post2dWindowGraphicsView*> (v);
	auto pos = view->viewportToWorld(event->pos());

	auto grid = gridDataItem()->grid();
	auto pointId = grid->vtkData()->findClosestPointWithinRadius(v->stdDistance(iRIC::nearRadius()), pos.x(), pos.y(), 0);
	if (pointId < 0) {
		v->setCursor(Qt::ArrowCursor);
	} else {
		v->setCursor(Qt::CrossCursor);
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
	m_setting.outlineActor()->SetPosition(0, 0, range.max());
	m_setting.wireframeActor()->SetPosition(0, 0, range.min());
}
