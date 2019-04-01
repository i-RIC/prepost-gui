#include "gridcreatingconditioncompoundchannel.h"
#include "gridcreatingconditioncompoundchannelabstractline.h"
#include "gridcreatingconditioncompoundchannelabstractpolygon.h"
#include "gridcreatingconditioncompoundchannelcenterline.h"
#include "gridcreatingconditioncompoundchannelfunctions.h"
#include "gridcreatingconditioncompoundchannelgridregionpolygon.h"
#include "gridcreatingconditioncompoundchannellowwaterchannelpolygon.h"
#include "gridcreatingconditioncompoundchannelsettingdialog.h"
#include "gridcreatingconditioncompoundchannelspline.h"

#include "private/gridcreatingconditioncompoundchannel_addpolygonvertexcommand.h"
#include "private/gridcreatingconditioncompoundchannel_addpolylinevertexcommand.h"
#include "private/gridcreatingconditioncompoundchannel_definepolygonnewpointcommand.h"
#include "private/gridcreatingconditioncompoundchannel_definepolylinenewpointcommand.h"
#include "private/gridcreatingconditioncompoundchannel_movepolygoncommand.h"
#include "private/gridcreatingconditioncompoundchannel_movepolygonvertexcommand.h"
#include "private/gridcreatingconditioncompoundchannel_movepolylinecommand.h"
#include "private/gridcreatingconditioncompoundchannel_movepolylinevertexcommand.h"
#include "private/gridcreatingconditioncompoundchannel_polygoncoordinateseditor.h"
#include "private/gridcreatingconditioncompoundchannel_polylinecoordinateseditor.h"
#include "private/gridcreatingconditioncompoundchannel_removepolygonvertexcommand.h"
#include "private/gridcreatingconditioncompoundchannel_removepolylinevertexcommand.h"
#include "private/gridcreatingconditioncompoundchannel_switchstatuscommand.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/errormessage.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/zdepthrange.h>

#include <QAction>
#include <QCoreApplication>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointF>
#include <QPolygonF>
#include <QTextStream>
#include <QToolBar>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define BANKSUBDIV 3
#define MIN_IT 5
#define EPS 0.002
#define MIN_CENTERLINEDIV 20

GridCreatingConditionCompoundChannel::GridCreatingConditionCompoundChannel(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	m_rightClickingMenu {nullptr}
{
	initParams();

	m_gridRegionPolygon = new GridCreatingConditionCompoundChannelGridRegionPolygon(this);
	m_gridRegionPolygon->setActive(true);
	m_selectedPolygon = m_gridRegionPolygon;

	m_lowWaterChannelPolygon = new GridCreatingConditionCompoundChannelLowWaterChannelPolygon(this);
	m_lowWaterChannelPolygon->setActive(false);

	m_centerLine = new GridCreatingConditionCompoundChannelCenterLine(this);
	m_centerLine->setActive(false);
	m_selectedLine = nullptr;

	m_addVertexAction = new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), tr("&Add Vertex"), this);
	m_addVertexAction->setCheckable(true);
	connect(m_addVertexAction, SIGNAL(triggered(bool)), this, SLOT(addVertexMode(bool)));
	m_removeVertexAction = new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), tr("&Remove Vertex"), this);
	m_removeVertexAction->setCheckable(true);
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), this, SLOT(removeVertexMode(bool)));
	m_coordEditAction = new QAction(tr("Edit C&oordinates..."), this);
	connect(m_coordEditAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));
	m_reverseCenterLineAction = new QAction(tr("R&everse Center Line Direction"), this);
	connect(m_reverseCenterLineAction, SIGNAL(triggered()), this, SLOT(reverseCenterLine()));

	// Set cursors for mouse view change events.
	m_addPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_removePixmap = QPixmap(":/libs/guibase/images/cursorRemove.png");
	m_addCursor = QCursor(m_addPixmap, 0, 0);
	m_removeCursor = QCursor(m_removePixmap, 0, 0);

	m_status = stDefiningRegion;
	m_mouseEventMode = meBeforeDefining;

	m_inhibitSelect = false;
	updateActionStatus();
}

GridCreatingConditionCompoundChannel::~GridCreatingConditionCompoundChannel()
{
	delete m_gridRegionPolygon;
	delete m_lowWaterChannelPolygon;
	delete m_centerLine;
	delete m_rightClickingMenu;
}

void GridCreatingConditionCompoundChannel::setupMenu()
{
	m_menu->addAction(m_addVertexAction);
	m_menu->addAction(m_removeVertexAction);
	m_menu->addAction(m_coordEditAction);
	m_menu->addAction(m_reverseCenterLineAction);
	m_menu->addSeparator();

	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(p->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addVertexAction);
	m_rightClickingMenu->addAction(m_removeVertexAction);
	m_rightClickingMenu->addAction(m_coordEditAction);
	m_rightClickingMenu->addAction(m_reverseCenterLineAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(p->clearAction());
}

bool GridCreatingConditionCompoundChannel::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(m_addVertexAction);
	tb->addAction(m_removeVertexAction);
	return true;
}

void GridCreatingConditionCompoundChannel::informSelection(PreProcessorGraphicsViewInterface* v)
{
	switch (m_selectMode) {
	case smPolygon:
		m_selectedPolygon->setActive(true);
		break;
	case smLine:
		m_selectedLine->setActive(true);
		break;
	case smNone:
		// do nothing.
		break;
	}
	updateMouseCursor(v);
}

void GridCreatingConditionCompoundChannel::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	switch (m_selectMode) {
	case smPolygon:
		m_selectedPolygon->setActive(false);
		break;
	case smLine:
		m_selectedLine->setActive(false);
		break;
	case smNone:
		// do nothing.
		break;
	}
	v->unsetCursor();
}

void GridCreatingConditionCompoundChannel::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GridCreatingConditionCompoundChannel::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}

	if (m_status == stDefiningRegion && m_mouseEventMode == meDefining) {
		definePolygon(false);
	}
	if (m_status == stDefiningLowWaterRegion && m_mouseEventMode == meDefining) {
		definePolygon(false);
	}
	if (m_status == stDefiningCenterLine && m_mouseEventMode == meDefining) {
		defineLine(false);
	}
}

void GridCreatingConditionCompoundChannel::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionCompoundChannel::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_status == stDefiningRegion && m_mouseEventMode == meDefining) {
		definePolygon(true);
	}
	if (m_status == stDefiningLowWaterRegion && m_mouseEventMode == meDefining) {
		definePolygon(true);
	}
	if (m_status == stDefiningCenterLine && m_mouseEventMode == meDefining) {
		defineLine(true);
	}
}

void GridCreatingConditionCompoundChannel::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (m_status == stDefiningRegion || m_status == stDefiningLowWaterRegion) {
		// defining a polygon
		// update the position of the last point.
		if (m_mouseEventMode == meDefining) {
			pushRenderCommand(new DefinePolygonNewPointCommand(false, event->pos(), this));
		}
	} else if (m_status == stDefiningCenterLine) {
		// defining a polyline.
		// update the position of the last point.
		if (m_mouseEventMode == meDefining) {
			pushRenderCommand(new DefinePolyLineNewPointCommand(false, event->pos(), this));
		}
	} else if (m_status == stNormal) {
		// defining stage finished.
		if (m_selectMode == smPolygon) {
			// a polygon is selected.
			switch (m_mouseEventMode) {
			case meNormal:
			case meTranslatePrepare:
			case meMoveVertexPrepare:
			case meAddVertexPrepare:
			case meAddVertexNotPossible:
			case meRemoveVertexPrepare:
			case meRemoveVertexNotPossible:
				m_currentPoint = event->pos();
				updateMouseEventMode();
				updateMouseCursor(v);
				break;
			case meBeforeDefining:
				// this does not happen
				break;
			case meDefining:
				break;
			case meTranslate:
				// execute translation.
				pushRenderCommand(new MovePolygonCommand(false, m_currentPoint, event->pos(), this));
				m_currentPoint = event->pos();
				break;
			case meMoveVertex:
				pushRenderCommand(new MovePolygonVertexCommand(false, m_currentPoint, event->pos(), m_selectedPolygon->selectedVertexId(), this));
				m_currentPoint = event->pos();
				break;
			case meAddVertex:
				pushRenderCommand(new AddPolygonVertexCommand(false, m_selectedPolygon->selectedEdgeId(), event->pos(), this));
				break;
			case meTranslateDialog:
				break;
			case meEditVerticesDialog:
				break;
			}
		} else if (m_selectMode == smLine) {
			// a line is selected.
			switch (m_mouseEventMode) {
			case meNormal:
			case meTranslatePrepare:
			case meMoveVertexPrepare:
			case meAddVertexPrepare:
			case meAddVertexNotPossible:
			case meRemoveVertexPrepare:
			case meRemoveVertexNotPossible:
				m_currentPoint = event->pos();
				updateMouseEventMode();
				updateMouseCursor(v);
				break;
			case meBeforeDefining:
				// this does not happen
				break;
			case meDefining:
				break;
			case meTranslate:
				// execute translation.
				pushRenderCommand(new MovePolyLineCommand(false, m_currentPoint, event->pos(), this));
				m_currentPoint = event->pos();
				break;
			case meMoveVertex:
				pushRenderCommand(new MovePolyLineVertexCommand(false, m_currentPoint, event->pos(), m_selectedLine->selectedVertexId(), this));
				m_currentPoint = event->pos();
				break;
			case meAddVertex:
				pushRenderCommand(new AddPolyLineVertexCommand(false, m_selectedLine->selectedEdgeId(), event->pos(), this));
				break;
			case meTranslateDialog:
				break;
			case meEditVerticesDialog:
				break;
			}
		} else {
			// nothing is selected.
		}
	}
}

void GridCreatingConditionCompoundChannel::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		// left click

		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);

		if (m_status == stDefiningRegion || m_status == stDefiningLowWaterRegion) {
			switch (m_mouseEventMode) {
			case meBeforeDefining:
				// enter defining mode.
				m_mouseEventMode = meDefining;
				pushRenderCommand(new DefinePolygonNewPointCommand(true, event->pos(), this));
			case meDefining:
				pushRenderCommand(new DefinePolygonNewPointCommand(true, event->pos(), this));
				break;
			default:
				break;
			}
			updateMouseCursor(v);
			updateActionStatus();
		} else if (m_status == stDefiningCenterLine) {
			// defining a polyline.
			switch (m_mouseEventMode) {
			case meBeforeDefining:
				// enter defining mode.
				m_mouseEventMode = meDefining;
				pushRenderCommand(new DefinePolyLineNewPointCommand(true, event->pos(), this));
			case meDefining:
				pushRenderCommand(new DefinePolyLineNewPointCommand(true, event->pos(), this));
				break;
			default:
				break;
			}
			updateMouseCursor(v);
			updateActionStatus();
		} else if (m_status == stNormal) {
			// defining stage finished.
			if (m_selectMode == smPolygon) {
				switch (m_mouseEventMode) {
				case meNormal:
					if (selectObject(event->pos())) {
						// selection changed.
						updateMouseEventMode();
						updateMouseCursor(v);
						updateActionStatus();
						renderGraphicsView();
					}
					break;
				case meBeforeDefining:
				case meDefining:
					// these does not happen.
					break;
				case meTranslatePrepare:
					if (selectObject(event->pos())) {
						// selection changed.
						updateMouseEventMode();
						updateMouseCursor(v);
						updateActionStatus();
						renderGraphicsView();
					} else {
						// start translating
						m_mouseEventMode = meTranslate;
						m_currentPoint = event->pos();
						// push the first translation command.
						pushRenderCommand(new MovePolygonCommand(true, m_currentPoint, m_currentPoint, this));
					}
					break;
				case meMoveVertexPrepare:
					m_mouseEventMode = meMoveVertex;
					m_currentPoint = event->pos();
					// push the first move command.
					pushRenderCommand(new MovePolygonVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedPolygon->selectedVertexId(), this));
					break;
				case meAddVertexPrepare:
					m_mouseEventMode = meAddVertex;
					pushRenderCommand(new AddPolygonVertexCommand(true, m_selectedPolygon->selectedEdgeId(), event->pos(), this));
					break;
				case meAddVertexNotPossible:
					// do nothing.
					break;
				case meRemoveVertexPrepare:
					if (m_selectedPolygon->polygon().count() == 1) {
						// you are going to remove the last point.
						// @todo
//						deletePolygon(true);
					} else {
						pushRenderCommand(new RemovePolygonVertexCommand(m_selectedPolygon->selectedVertexId(), this));
					}
					m_inhibitSelect = true;
					break;
				case meRemoveVertexNotPossible:
					// do nothing.
					break;
				case meTranslate:
					// this should not happen.
					break;
				case meMoveVertex:
					// this should not happen.
					break;
				case meAddVertex:
					// this should not happen.
					break;
				case meTranslateDialog:
					break;
				case meEditVerticesDialog:
					break;
				}
				updateMouseCursor(v);
				updateActionStatus();
			} else if (m_selectMode == smLine) {
				switch (m_mouseEventMode) {
				case meNormal:
					if (selectObject(event->pos())) {
						// selection changed.
						updateMouseEventMode();
						updateMouseCursor(v);
						updateActionStatus();
						renderGraphicsView();
					}
					break;
				case meBeforeDefining:
				case meDefining:
					// this should not happen.
					break;
				case meTranslatePrepare:
					if (selectObject(event->pos())) {
						// selection changed.
						updateMouseEventMode();
						updateMouseCursor(v);
						updateActionStatus();
						renderGraphicsView();
					} else {
						// start translating
						m_mouseEventMode = meTranslate;
						m_currentPoint = event->pos();
						// push the first translation command.
						pushRenderCommand(new MovePolyLineCommand(true, m_currentPoint, m_currentPoint, this));
					}
					break;
				case meMoveVertexPrepare:
					m_mouseEventMode = meMoveVertex;
					m_currentPoint = event->pos();
					// push the first move command.
					pushRenderCommand(new MovePolyLineVertexCommand(true, m_currentPoint, m_currentPoint, m_selectedLine->selectedVertexId(), this));
					break;
				case meAddVertexPrepare:
					m_mouseEventMode = meAddVertex;
					pushRenderCommand(new AddPolyLineVertexCommand(true, m_selectedLine->selectedEdgeId(), event->pos(), this));
					break;
				case meAddVertexNotPossible:
					// do nothing.
					break;
				case meRemoveVertexPrepare:
					if (m_selectedLine->polyLine().count() == 1) {
						// ignore.
					} else {
						pushRenderCommand(new RemovePolyLineVertexCommand(m_selectedLine->selectedVertexId(), this));
					}
					m_inhibitSelect = true;
					break;
				case meRemoveVertexNotPossible:
					// do nothing.
					break;
				case meTranslate:
					// this should not happen.
					break;
				case meMoveVertex:
					// this should not happen.
					break;
				case meAddVertex:
					// this should not happen.
					break;
				case meTranslateDialog:
					break;
				case meEditVerticesDialog:
					break;
				}
				updateMouseCursor(v);
				updateActionStatus();
			} else {
				// nothing is selected.

				// tries to select something.
				if (selectObject(event->pos())) {
					// selection changed.
					updateMouseEventMode();
					updateMouseCursor(v);
					updateActionStatus();
					renderGraphicsView();
				}
			}
		}
	} else if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = event->pos();
	}
}

void GridCreatingConditionCompoundChannel::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (m_mouseEventMode) {
		case meNormal:
		case meTranslatePrepare:
		case meMoveVertexPrepare:
		case meAddVertexPrepare:
		case meAddVertexNotPossible:
		case meRemoveVertexPrepare:
		case meRemoveVertexNotPossible:
		case meTranslate:
		case meMoveVertex:
		case meAddVertex:
			m_currentPoint = event->pos();
			updateMouseEventMode();
			updateMouseCursor(v);
			updateActionStatus();
			break;
		case meBeforeDefining:
		case meDefining:
			// do nothing no mode change.
			updateMouseCursor(v);
			break;
		case meTranslateDialog:
			break;
		case meEditVerticesDialog:
			break;
		}
		m_inhibitSelect = false;
	} else if (event->button() == Qt::RightButton) {
		if (m_mouseEventMode == meEditVerticesDialog) {return;}
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionCompoundChannel::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meAddVertexNotPossible:
	case meRemoveVertexNotPossible:
	case meTranslateDialog:
	case meEditVerticesDialog:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBeforeDefining:
	case meDefining:
		v->setCursor(Qt::CrossCursor);
		break;
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case meAddVertexPrepare:
		v->setCursor(m_addCursor);
		break;
	case meRemoveVertexPrepare:
		v->setCursor(m_removeCursor);
		break;
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	}
}

void GridCreatingConditionCompoundChannel::addCustomMenuItems(QMenu* /*menu*/)
{}

void GridCreatingConditionCompoundChannel::definePolygon(bool doubleClick)
{
	int minCount = 4;
	if (doubleClick) {
		minCount = 3;
	}
	if (m_selectedPolygon == nullptr) {return;}
	if (m_selectedPolygon->polygon().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polygon must have three vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	stack.beginMacro(tr("Finish Defining Polygon"));
	// finish defining the polygon.
	if (m_status == stDefiningRegion) {
		pushRenderCommand(new SwitchStatusCommand(stDefiningLowWaterRegion, this));
	} else if (m_status == stDefiningLowWaterRegion) {
		pushRenderCommand(new SwitchStatusCommand(stDefiningCenterLine, this));
	}
	stack.endMacro();
}

void GridCreatingConditionCompoundChannel::defineLine(bool doubleClick)
{
	int minCount = 2;
	if (doubleClick) {
		minCount = 1;
	}
	if (m_selectedLine == nullptr) {return;}
	if (m_selectedLine->polyLine().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polygonal line must have two vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	stack.beginMacro(tr("Finish Defining Polygonal line"));
	// finish defining the line.
	pushRenderCommand(new SwitchStatusCommand(stNormal, this));
	stack.endMacro();
}

const QColor& GridCreatingConditionCompoundChannel::color() const
{
	return m_color;
}

void GridCreatingConditionCompoundChannel::addVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meAddVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionCompoundChannel::removeVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionCompoundChannel::doLoadFromProjectMainFile(const QDomNode& /*node*/) {}

void GridCreatingConditionCompoundChannel::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) {}

void GridCreatingConditionCompoundChannel::doApplyOffset(double x, double y)
{
	m_gridRegionPolygon->setPolygon(m_gridRegionPolygon->polygon(QPointF(x, y)));
	m_lowWaterChannelPolygon->setPolygon(m_lowWaterChannelPolygon->polygon(QPointF(x, y)));
	m_centerLine->setPolyLine(m_centerLine->polyLine(QPointF(x, y)));
}

void GridCreatingConditionCompoundChannel::loadExternalData(const QString& filename)
{
	auto off = offset();
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);
	QVector<QPointF> points;
	s >> m_streamWiseDivision >> m_leftDivision >> m_centerDivision >>  m_rightDivision;
	QPolygonF poly;
	s >> poly;
	m_gridRegionPolygon->setPolygon(poly);
	m_gridRegionPolygon->setPolygon(m_gridRegionPolygon->polygon(QPointF(off.x(), off.y())));
	QPolygonF poly2;
	s >> poly2;
	m_lowWaterChannelPolygon->setPolygon(poly2);
	m_lowWaterChannelPolygon->setPolygon(m_lowWaterChannelPolygon->polygon(QPointF(off.x(), off.y())));
	s >> points;
	m_centerLine->setPolyLine(points);
	m_centerLine->setPolyLine(m_centerLine->polyLine(QPointF(off.x(), off.y())));
	m_selectMode = smNone;
	m_selectedPolygon = nullptr;
	m_selectedLine = nullptr;
	updateActionStatus();
	f.close();

	if (m_gridRegionPolygon->polygon().count() == 0) {
		// grid region not defined yet.
		m_status = stDefiningRegion;
		m_mouseEventMode = meBeforeDefining;
		m_selectMode = smPolygon;
		m_selectedPolygon = m_gridRegionPolygon;
		m_gridRegionPolygon->setActive(true);
		m_lowWaterChannelPolygon->setActive(false);
		m_centerLine->setActive(false);
	} else if (m_lowWaterChannelPolygon->polygon().count() == 0) {
		// low water channel not defined yet.
		m_status = stDefiningLowWaterRegion;
		m_mouseEventMode = meBeforeDefining;
		m_selectMode = smPolygon;
		m_selectedPolygon = m_lowWaterChannelPolygon;
		m_gridRegionPolygon->setActive(false);
		m_lowWaterChannelPolygon->setActive(true);
		m_centerLine->setActive(false);
	} else if (m_centerLine->polyLine().count() == 0) {
		// center line not defined yet.
		m_status = stDefiningCenterLine;
		m_mouseEventMode = meBeforeDefining;
		m_selectMode = smLine;
		m_selectedLine = m_centerLine;
		m_gridRegionPolygon->setActive(false);
		m_lowWaterChannelPolygon->setActive(false);
		m_centerLine->setActive(true);
	} else {
		// definition finished.
		m_status = stNormal;
		m_mouseEventMode = meNormal;
		m_selectMode = smNone;
		m_gridRegionPolygon->setActive(false);
		m_lowWaterChannelPolygon->setActive(false);
		m_centerLine->setActive(false);
	}
}

void GridCreatingConditionCompoundChannel::saveExternalData(const QString& filename)
{
	auto off = offset();
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	s << m_streamWiseDivision << m_leftDivision << m_centerDivision << m_rightDivision;

	s << m_gridRegionPolygon->polygon(QPointF(-off.x(), -off.y()));
	s << m_lowWaterChannelPolygon->polygon(QPointF(-off.x(), -off.y()));
	s << m_centerLine->polyLine(QPointF(-off.x(), -off.y()));
	f.close();
}

void GridCreatingConditionCompoundChannel::updateFilename()
{
	setFilename("gridcreatingcondition.dat");
}

void GridCreatingConditionCompoundChannel::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GridCreatingConditionCompoundChannel::assignActorZValues(const ZDepthRange& range)
{
	m_depthRange = range;
	m_gridRegionPolygon->setZDepthRange(range.min(), range.max());
	m_lowWaterChannelPolygon->setZDepthRange(range.min(), range.max());
	m_centerLine->setZDepthRange(range.min(), range.max());
}

void GridCreatingConditionCompoundChannel::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QVector2D worldPos(dx, dy);
	switch (m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (m_selectedPolygon->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meAddVertexPrepare;
			} else {
				m_mouseEventMode = meAddVertexNotPossible;
			}
		}
		if (m_selectMode == smLine) {
			if (m_selectedLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meAddVertexPrepare;
			} else {
				m_mouseEventMode = meAddVertexNotPossible;
			}
		}
		break;
	case meRemoveVertexNotPossible:
	case meRemoveVertexPrepare:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meRemoveVertexPrepare;
			} else {
				m_mouseEventMode = meRemoveVertexNotPossible;
			}
		}
		if (m_selectMode == smLine) {
			if (m_selectedLine->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meRemoveVertexPrepare;
			} else {
				m_mouseEventMode = meRemoveVertexNotPossible;
			}
		}
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		if (m_selectMode == smNone) {return;}
		if (m_selectMode == smPolygon) {
			if (m_selectedPolygon->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meMoveVertexPrepare;
			} else if (m_selectedPolygon->isPolygonSelectable(worldPos)) {
				m_mouseEventMode = meTranslatePrepare;
			} else {
				m_mouseEventMode = meNormal;
			}
		}
		if (m_selectMode == smLine) {
			if (m_selectedLine->isVertexSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meMoveVertexPrepare;
			} else if (m_selectedLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meTranslatePrepare;
			} else {
				m_mouseEventMode = meNormal;
			}
		}
		break;
	case meBeforeDefining:
	case meDefining:
		// do nothing
		break;
	case meTranslateDialog:
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCompoundChannel::updateActionStatus()
{
	switch (m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		m_reverseCenterLineAction->setEnabled(false);
		break;
	case meTranslate:
	case meMoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		m_reverseCenterLineAction->setEnabled(false);
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setChecked(false);

		if (m_status == stNormal && m_selectMode != smNone) {
			m_addVertexAction->setEnabled(true);
			if (m_selectMode == smPolygon) {
				m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
			} else if (m_selectMode == smLine) {
				m_removeVertexAction->setEnabled(activePolylineHasThreeVertices());
			}
			m_coordEditAction->setEnabled(true);
		} else {
			m_addVertexAction->setDisabled(true);
			m_removeVertexAction->setDisabled(true);
			m_coordEditAction->setDisabled(true);
		}
		m_reverseCenterLineAction->setEnabled(true);
		break;
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meAddVertex:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setEnabled(activePolygonHasFourVertices());
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		m_reverseCenterLineAction->setEnabled(true);
		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		m_coordEditAction->setEnabled(false);
		m_reverseCenterLineAction->setEnabled(true);
		break;
	case meTranslateDialog:
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCompoundChannel::reverseCenterLine()
{
	m_centerLine->reverseDirection();
	renderGraphicsView();
}

void GridCreatingConditionCompoundChannel::editCoordinates()
{
	if (m_selectedPolygon != nullptr) {
		PolygonCoordinatesEditor::edit(this);
	} else if (m_selectedLine != nullptr) {
		PolylineCoordinatesEditor::edit(this);
	}
}

void GridCreatingConditionCompoundChannel::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

void GridCreatingConditionCompoundChannel::cancel()
{
	m_canceled = true;
}

bool GridCreatingConditionCompoundChannel::create(QWidget* parent)
{
	if (! checkCondition()) {return false;}
	GridCreatingConditionCompoundChannelSettingDialog dialog(parent);
	dialog.setStreamWiseDivision(m_streamWiseDivision);
	dialog.setLeftDivision(m_leftDivision);
	dialog.setRightDivision(m_rightDivision);
	dialog.setCenterDivision(m_centerDivision);

	dialog.setRelaxation(m_relaxation);
	dialog.setIterations(m_iterations);

	if (QDialog::Rejected == dialog.exec()) {
		return false;
	}
	m_streamWiseDivision = dialog.streamWiseDivision();
	m_leftDivision = dialog.leftDivision();
	m_rightDivision = dialog.rightDivision();
	m_centerDivision = dialog.centerDivision();
	m_relaxation = dialog.relaxation();
	m_iterations = dialog.iterations();

	Grid* grid = createGrid();
	if (grid == nullptr) {return false;}
	emit gridCreated(grid);
	return true;
}

void GridCreatingConditionCompoundChannel::clear()
{
	initParams();

	delete m_gridRegionPolygon;
	m_gridRegionPolygon = new GridCreatingConditionCompoundChannelGridRegionPolygon(this);
	m_gridRegionPolygon->setActive(true);
	m_selectedPolygon = m_gridRegionPolygon;

	delete m_lowWaterChannelPolygon;
	m_lowWaterChannelPolygon = new GridCreatingConditionCompoundChannelLowWaterChannelPolygon(this);
	m_lowWaterChannelPolygon->setActive(false);

	delete m_centerLine;
	m_centerLine = new GridCreatingConditionCompoundChannelCenterLine(this);
	m_centerLine->setActive(false);
	m_selectedLine = nullptr;

	m_status = stDefiningRegion;
	m_mouseEventMode = meBeforeDefining;

	updateMouseCursor(graphicsView());
	updateActionStatus();
}

bool GridCreatingConditionCompoundChannel::ready() const
{
	return true;
}

void GridCreatingConditionCompoundChannel::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionCompoundChannel::tr("Information"), GridCreatingConditionCompoundChannel::tr("Please define grid creating region. Grid creating region can be defined as polygon by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gccompoundchannel_gridregion");
}

void GridCreatingConditionCompoundChannel::initParams()
{
	m_streamWiseDivision = 20;
	m_leftDivision = 3;
	m_rightDivision = 3;
	m_centerDivision = 5;

	m_relaxation = 0.15;
	m_iterations = 30;

}

Grid* GridCreatingConditionCompoundChannel::createGrid()
{
	// first, setup the centerline.
	int pointsNum = m_streamWiseDivision * BANKSUBDIV;
	if (pointsNum < MIN_CENTERLINEDIV) {pointsNum = MIN_CENTERLINEDIV;}

	GridCreatingConditionCompoundChannelSpline centerSpline;
	centerSpline.setPoints(m_centerLine->polyLine());
	int Nx = m_streamWiseDivision + 1;
	QVector<QPointF> centerLinePoints = centerSpline.getEquallySpacedPoints(pointsNum);

	// setup left bank.
	GridCreatingConditionCompoundChannelSpline leftSpline = getLeftBank(centerLinePoints);
	leftSpline.setupPoints(pointsNum);

	// setup right bank.
	GridCreatingConditionCompoundChannelSpline rightSpline = getRightBank(centerLinePoints);
	rightSpline.setupPoints(pointsNum);

	// setup left edge.
	GridCreatingConditionCompoundChannelSpline leftEdge = getLeftEdge(leftSpline.getEquallySpacedPoints(Nx * BANKSUBDIV));
	leftEdge.setupPoints(pointsNum);

	// setup right edge.
	GridCreatingConditionCompoundChannelSpline rightEdge = getRightEdge(rightSpline.getEquallySpacedPoints(Nx * BANKSUBDIV));
	rightEdge.setupPoints(pointsNum);

	return createGridCore(leftEdge, leftSpline, centerSpline, rightSpline, rightEdge);
}

Grid* GridCreatingConditionCompoundChannel::createGridCore(const GridCreatingConditionCompoundChannelSpline& leftEdge, const GridCreatingConditionCompoundChannelSpline& leftBank, const GridCreatingConditionCompoundChannelSpline& centerLine, const GridCreatingConditionCompoundChannelSpline& rightBank, const GridCreatingConditionCompoundChannelSpline& rightEdge)
{
	int isize = m_streamWiseDivision + 1;
	int jsize = m_leftDivision + m_rightDivision + m_centerDivision + 1;
	GridCreatingConditionCompoundChannelTemporaryGrid tmpGrid(isize, jsize);

	m_canceled = false;
	WaitDialog waitDialog(preProcessorWindow());
	waitDialog.showProgressBar();
	waitDialog.setRange(0, m_iterations);
	waitDialog.setProgress(0);
	waitDialog.setMessage(tr("Generating grid..."));
	connect(&waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
	waitDialog.show();
	setupInitialGrid(tmpGrid, leftEdge, leftBank, centerLine, rightBank, rightEdge);
	qApp->processEvents();

	for (int n = 0; n < m_iterations; ++n) {
		relaxGrid(tmpGrid, m_relaxation);
		applyConstraintToGrid(tmpGrid, leftEdge, leftBank, centerLine, rightBank, rightEdge);
		qApp->processEvents();
		if (m_canceled) {return nullptr;}
		waitDialog.setProgress(n + 1);
	}
	waitDialog.hide();
	Structured2DGrid* grid = new Structured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);
	grid->setDimensions(isize, jsize);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	for (int j = 0; j < jsize; ++j) {
		for (int i = 0; i < isize; ++i) {
			QPointF p = tmpGrid.point(i, j);
			points->InsertNextPoint(p.x(), p.y(), 0);
		}
	}
	grid->vtkGrid()->SetPoints(points);
	points->Delete();

	// allocate memory for all grid related conditions.
	for (GridAttributeContainer* c : grid->gridAttributes()) {
		c->allocate();
	}
	grid->setModified();
	return grid;
}

void GridCreatingConditionCompoundChannel::setupInitialGrid(GridCreatingConditionCompoundChannelTemporaryGrid& grid, const GridCreatingConditionCompoundChannelSpline& leftEdge, const GridCreatingConditionCompoundChannelSpline& leftBank, const GridCreatingConditionCompoundChannelSpline& centerLine, const GridCreatingConditionCompoundChannelSpline& rightBank, const GridCreatingConditionCompoundChannelSpline& rightEdge)
{
	// setup centerline points
	QVector<QPointF> centerLinePoints = centerLine.getEquallySpacedPoints(m_streamWiseDivision);

	// setup left bank points
	QVector<QPointF> leftBankPoints;
	int i;
	for (i = 0; i < centerLinePoints.count() - 1; ++i) {
		QPointF centerP = centerLinePoints.at(i);
		QPointF toLeft = iRIC::rotateVector90(iRIC::normalize(centerLinePoints.at(i + 1) - centerLinePoints.at(i)));
		QPointF leftP = centerP + toLeft;
		QPointF leftBankP = leftBank.getIntersectPoint(QLineF(centerP, leftP));
		if (leftBankP == GridCreatingConditionCompoundChannelSpline::errorPoint) {
			leftBankP = leftBank.getNearestPoint(centerP);
		}
		leftBankPoints.append(leftBankP);
	}
	QPointF centerP = centerLinePoints.at(i);
	QPointF toLeft = iRIC::rotateVector270(iRIC::normalize(centerLinePoints.at(i - 1) - centerLinePoints.at(i)));
	QPointF leftP = centerP + toLeft;
	QPointF leftBankP = leftBank.getIntersectPoint(QLineF(centerP, leftP));
	if (leftBankP == GridCreatingConditionCompoundChannelSpline::errorPoint) {
		leftBankP = leftBank.getNearestPoint(centerP);
	}
//	QString fname("leftbank_%1.vtk");
//	QFile::rename("log.vtk", fname.arg(i));
	leftBankPoints.append(leftBankP);

	// setup right bank points
	QVector<QPointF> rightBankPoints;
	for (i = 0; i < centerLinePoints.count() - 1; ++i) {
		QPointF centerP = centerLinePoints.at(i);
		QPointF toRight = iRIC::rotateVector270(iRIC::normalize(centerLinePoints.at(i + 1) - centerLinePoints.at(i)));
		QPointF rightP = centerP + toRight;
		QPointF rightBankP = rightBank.getIntersectPoint(QLineF(centerP, rightP));
		if (rightBankP == GridCreatingConditionCompoundChannelSpline::errorPoint) {
			rightBankP = rightBank.getNearestPoint(centerP);
		}
		rightBankPoints.append(rightBankP);
	}
	centerP = centerLinePoints.at(i);
	QPointF toRight = iRIC::rotateVector90(iRIC::normalize(centerLinePoints.at(i - 1) - centerLinePoints.at(i)));
	QPointF rightP = centerP + toRight;
	QPointF rightBankP = rightBank.getIntersectPoint(QLineF(centerP, rightP));
	if (rightBankP == GridCreatingConditionCompoundChannelSpline::errorPoint) {
		rightBankP = rightBank.getNearestPoint(centerP);
	}
	rightBankPoints.append(rightBankP);

	// setup left edge points.
	QVector<QPointF> leftEdgePoints;
	for (int i = 0; i < leftBankPoints.count(); ++i) {
		QPointF leftP = leftBankPoints.at(i);
		QPointF newP = leftEdge.getNearestPoint(leftP);
		leftEdgePoints.append(newP);
	}

	// setup right edge points.
	QVector<QPointF> rightEdgePoints;
	for (int i = 0; i < rightBankPoints.count(); ++i) {
		QPointF rightP = rightBankPoints.at(i);
		QPointF newP = rightEdge.getNearestPoint(rightP);
		rightEdgePoints.append(newP);
	}

	// now, setup the grid.
	for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
		grid.point(i, 0) = rightEdgePoints.at(i);
	}
	for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
		grid.point(i, m_rightDivision) = rightBankPoints.at(i);
	}
	for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
		grid.point(i, m_rightDivision + m_centerDivision) = leftBankPoints.at(i);
	}
	for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
		grid.point(i, m_rightDivision + m_centerDivision + m_leftDivision) = leftEdgePoints.at(i);
	}
	for (int j = 1; j < m_rightDivision; ++ j) {
		double param = j / static_cast<double>(m_rightDivision);
		for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
			QPointF p = param * rightBankPoints.at(i) + (1 - param) * rightEdgePoints.at(i);
			grid.point(i, j) = p;
		}
	}
	for (int j = 1; j < m_centerDivision; ++j) {
		double param = j / static_cast<double>(m_centerDivision);
		for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
			QPointF p = param * leftBankPoints.at(i) + (1 - param) * rightBankPoints.at(i);
			grid.point(i, j + m_rightDivision) = p;
		}
	}
	for (int j = 1; j < m_leftDivision; ++ j) {
		double param = j / static_cast<double>(m_leftDivision);
		for (int i = 0; i < m_streamWiseDivision + 1; ++i) {
			QPointF p = param * leftEdgePoints.at(i) + (1 - param) * leftBankPoints.at(i);
			grid.point(i, j + m_rightDivision + m_centerDivision) = p;
		}
	}
}

void GridCreatingConditionCompoundChannel::relaxGrid(GridCreatingConditionCompoundChannelTemporaryGrid& grid, double relax)
{
	GridCreatingConditionCompoundChannelTemporaryGrid grid2 = grid;
	for (int j = 1; j < grid.jsize() - 1; ++j) {
		for (int i = 1; i < grid.isize() - 1; ++i) {
//			QPointF Pfl = (grid.point(i, j) + grid.point(i + 1, j) + grid.point(i, j + 1) + grid.point(i + 1, j + 1)) / 4;
//			QPointF Pfr = (grid.point(i, j) + grid.point(i + 1, j) + grid.point(i, j - 1) + grid.point(i + 1, j - 1)) / 4;
//			QPointF Pbl = (grid.point(i, j) + grid.point(i - 1, j) + grid.point(i, j + 1) + grid.point(i - 1, j + 1)) / 4;
//			QPointF Pbr = (grid.point(i, j) + grid.point(i - 1, j) + grid.point(i, j - 1) + grid.point(i - 1, j - 1)) / 4;

//			double G1f = iRIC::distance(grid.point(i + 1, j), grid.point(i, j));
//			double G1b = iRIC::distance(grid.point(i - 1, j), grid.point(i, j));
//			double G1l = iRIC::distance(Pfl, Pbl);
//			double G1r = iRIC::distance(Pfr, Pbr);
//			double G2f = iRIC::distance(Pfl, Pfr);
//			double G2b = iRIC::distance(Pbl, Pbr);
//			double G2l = iRIC::distance(grid.point(i, j + 1), grid.point(i, j));
//			double G2r = iRIC::distance(grid.point(i, j - 1), grid.point(i, j));
//			double Af = G2f / G1f;
//			double Ab = G2b / G1b;
//			double Al = G1l / G2l;
//			double Ar = G1r / G2r;

			QPointF Pf = (grid.point(i, j) + grid.point(i + 1, j)) / 2;
			QPointF Pb = (grid.point(i, j) + grid.point(i - 1, j)) / 2;
			QPointF Pl = (grid.point(i, j) + grid.point(i, j + 1)) / 2;
			QPointF Pr = (grid.point(i, j) + grid.point(i, j - 1)) / 2;

			double G1 = iRIC::distance(Pf, Pb);
			double G2 = iRIC::distance(Pl, Pr);
			double Afy = G2 / G1;
			double Aby = G2 / G1;
			double Aly = G1 / G2;
			double Ary = G1 / G2;

//			double Tra = (Pf.x() - Pb.x()) * (Pl.y() - Pr.y()) - (Pl.x() - Pr.x()) * (Pf.y() - Pb.y());
//			double Tra2 = Tra * Tra;
			QPointF newP = (Afy * grid.point(i + 1, j) + Aby * grid.point(i - 1, j) + Aly * grid.point(i, j + 1) + Ary * grid.point(i, j - 1)) /
										 (Afy + Aby + Aly + Ary);
			// use relaxed point.
			QPointF newP2 = relax * newP + (1 - relax) * grid.point(i, j);
			grid2.point(i, j) = newP2;
		}
	}
	for (int i = 1; i < grid.isize() - 1; ++i) {
		QPointF newP = (grid.point(i - 1, 0) + grid.point(i + 1, 0)) / 2;
		QPointF newP2 = relax * newP + (1 - relax) * grid.point(i, 0);
		grid2.point(i, 0) = newP2;
	}
	for (int i = 1; i < grid.isize() - 1; ++i) {
		QPointF newP = (grid.point(i - 1, grid.jsize() - 1) + grid.point(i + 1, grid.jsize() - 1)) / 2;
		QPointF newP2 = relax * newP + (1 - relax) * grid.point(i, grid.jsize() - 1);
		grid2.point(i, grid.jsize() - 1) = newP2;
	}
	grid = grid2;
}

void GridCreatingConditionCompoundChannel::applyConstraintToGrid(GridCreatingConditionCompoundChannelTemporaryGrid& grid, const GridCreatingConditionCompoundChannelSpline& leftEdge, const GridCreatingConditionCompoundChannelSpline& leftBank, const GridCreatingConditionCompoundChannelSpline& /*centerLine*/, const GridCreatingConditionCompoundChannelSpline& rightBank, const GridCreatingConditionCompoundChannelSpline& rightEdge)
{
	for (int i = 1; i < m_streamWiseDivision; ++i) {
		QPointF p = rightEdge.getNearestPoint(grid.point(i, 0));
		grid.point(i, 0) = p;
	}
	for (int i = 1; i < m_streamWiseDivision; ++i) {
		QPointF p = rightBank.getNearestPoint(grid.point(i, m_rightDivision));
		grid.point(i, m_rightDivision) = p;
	}
	for (int i = 1; i < m_streamWiseDivision; ++i) {
		QPointF p = leftBank.getNearestPoint(grid.point(i, m_rightDivision + m_centerDivision));
		grid.point(i, m_rightDivision + m_centerDivision) = p;
	}
	for (int i = 1; i < m_streamWiseDivision; ++i) {
		QPointF p = leftEdge.getNearestPoint(grid.point(i, m_rightDivision + m_centerDivision + m_leftDivision));
		grid.point(i, m_rightDivision + m_centerDivision + m_leftDivision) = p;
	}
}


bool GridCreatingConditionCompoundChannel::selectObject(QPoint point)
{
	SelectMode oldSelectMode = m_selectMode;
	GridCreatingConditionCompoundChannelAbstractPolygon* oldSelectedPolygon = m_selectedPolygon;
	GridCreatingConditionCompoundChannelAbstractLine* oldSelectedLine = m_selectedLine;
	if (oldSelectMode == smPolygon) {oldSelectedPolygon->setActive(false);}
	if (oldSelectMode == smLine) {oldSelectedLine->setActive(false);}

	double dx = point.x();
	double dy = point.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF p(dx, dy);
	QVector2D pv(dx, dy);

	double selectlimit = graphicsView()->stdRadius(iRIC::nearRadius());

	// check whether the center line can be selected.
	bool selected = false;
	if (m_centerLine->isEdgeSelectable(pv, selectlimit)) {
		m_selectMode = smLine;
		m_selectedLine = m_centerLine;
		m_selectedLine->setActive(true);
		m_selectedPolygon = nullptr;
		selected = true;
	}

	// check whether the low water channel region can be selected.
	if (! selected) {
		QPolygonF polF = m_lowWaterChannelPolygon->polygon();
		if (polF.count() < 3) {
			// it contains only one or two points.
			if (m_lowWaterChannelPolygon->isEdgeSelectable(pv, selectlimit) || m_lowWaterChannelPolygon->isVertexSelectable(pv, selectlimit)) {
				selected = true;
			}
		} else {
			if (polF.containsPoint(p, Qt::OddEvenFill)) {
				selected = true;
			}
		}
		if (selected) {
			m_selectMode = smPolygon;
			m_selectedPolygon = m_lowWaterChannelPolygon;
			m_selectedPolygon->setActive(true);
			m_selectedLine = nullptr;
		}
	}
	// check whether the grid region can be selected.
	if (! selected) {
		QPolygonF polF = m_gridRegionPolygon->polygon();
		if (polF.count() < 3) {
			// it contains only one or two points.
			if (m_gridRegionPolygon->isEdgeSelectable(pv, selectlimit) || m_gridRegionPolygon->isVertexSelectable(pv, selectlimit)) {
				selected = true;
			}
		} else {
			if (polF.containsPoint(p, Qt::OddEvenFill)) {
				selected = true;
			}
		}
		if (selected) {
			m_selectMode = smPolygon;
			m_selectedPolygon = m_gridRegionPolygon;
			m_selectedPolygon->setActive(true);
			m_selectedLine = nullptr;
		}
	}
	if (! selected) {
		m_selectMode = smNone;
		m_selectedLine = nullptr;
		m_selectedPolygon = nullptr;
	}
	if (m_selectMode != oldSelectMode) {return true;}
	if (m_selectMode == smPolygon) {
		return m_selectedPolygon != oldSelectedPolygon;
	}
	if (m_selectMode == smLine) {
		return m_selectedLine != oldSelectedLine;
	}
	return false;
}

bool GridCreatingConditionCompoundChannel::checkCondition()
{
	QPolygonF region = m_gridRegionPolygon->polygon();
	QPolygonF low = m_lowWaterChannelPolygon->polygon();
	QVector<QPointF> centerLine = m_centerLine->polyLine();

	if (m_status != stNormal) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid creating condition definition not finished yet."));
		return false;
	}

	if (iRIC::hasIntersection(region)) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid creating region polygon shape is invalid."));
		return false;
	}
	if (iRIC::hasIntersection(low)) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Low water channel region polygon shape is invalid."));
		return false;
	}
	for (int i = 0; i < low.count(); ++i) {
		QPointF point = low.at(i);
		if (! region.containsPoint(point, Qt::OddEvenFill)) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Low water channel region have to be inside grid region."));
			return false;
		}
	}
	for (int i = 0; i < centerLine.count(); ++i) {
		QPointF point = centerLine.at(i);
		if (! low.containsPoint(point, Qt::OddEvenFill)) {
			QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Center line have to be inside low water channel region."));
			return false;
		}
	}
	return true;
}

bool GridCreatingConditionCompoundChannel::activePolygonHasFourVertices()
{
	if (m_selectedPolygon == nullptr) {return false;}
	QPolygonF pol = m_selectedPolygon->polygon();
	return pol.count() > 4;
}

bool GridCreatingConditionCompoundChannel::activePolylineHasThreeVertices()
{
	if (m_selectedLine == nullptr) {return false;}
	QVector<QPointF> line = m_selectedLine->polyLine();
	return line.count() >= 3;
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getLeftBank(const QVector<QPointF>& centerLine)
{
	QPointF p;

	// centerToLeft vector at head
	QPointF headCenterToLeft = centerLine[1] - centerLine[0];
	iRIC::rotateVector(headCenterToLeft, 100);

	int headEdge   = m_lowWaterChannelPolygon->getEdgeThatIntersect(QLineF(centerLine[0], centerLine[0] + QPointF(headCenterToLeft.x(), headCenterToLeft.y())));
	bool discardHead = false;
	vtkCell* cell = m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(headEdge);
	headCenterToLeft = centerLine[1] - centerLine[0];
	iRIC::rotateVector90(headCenterToLeft);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - centerLine[0].x(), x[1] - centerLine[0].y());
		if (QPointF::dotProduct(headCenterToLeft, tmp) < 0) {
			discardHead = true;
		}
	}

	// centerToLeft vector at middle
	int count = centerLine.count();
	int middleCount = (count - 1) / 2;
	QPointF middleCenterToLeft = centerLine[middleCount + 1] - centerLine[middleCount];
	iRIC::rotateVector90(middleCenterToLeft);

	int middleEdge = m_lowWaterChannelPolygon->getEdgeThatIntersect(QLineF(centerLine[middleCount], centerLine[middleCount] + QPointF(middleCenterToLeft.x(), middleCenterToLeft.y())));

	// centerToLeft vector at tail
	QPointF tailCenterToLeft = centerLine[count - 1] - centerLine[count - 2];
	iRIC::rotateVector(tailCenterToLeft, 80);

	int tailEdge   = m_lowWaterChannelPolygon->getEdgeThatIntersect(QLineF(centerLine[count - 1], centerLine[count - 1] + QPointF(tailCenterToLeft.x(), tailCenterToLeft.y())));

	bool discardTail = false;
	cell = m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(tailEdge);
	tailCenterToLeft = centerLine[count - 1] - centerLine[count - 2];
	iRIC::rotateVector90(tailCenterToLeft);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - centerLine[count - 1].x(), x[1] - centerLine[count - 1].y());
		if (QPointF::dotProduct(tailCenterToLeft, tmp) < 0) {
			discardTail = true;
		}
	}

	QPointF hintVec = centerLine[count - 1] - centerLine[0];
	return getLowWaterEdgeSpline(headEdge, middleEdge, tailEdge, discardHead, discardTail, QVector2D(hintVec.x(), hintVec.y()));
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getRightBank(const QVector<QPointF>& centerLine)
{
	// centerToRight vector at head
	QPointF headCenterToRight = centerLine[1] - centerLine[0];
	iRIC::rotateVector(headCenterToRight, 260);

	int headEdge   = m_lowWaterChannelPolygon->getEdgeThatIntersect(QLineF(centerLine[0], centerLine[0] + QPointF(headCenterToRight.x(), headCenterToRight.y())));
	bool discardHead = false;
	vtkCell* cell = m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(headEdge);
	headCenterToRight = centerLine[1] - centerLine[0];
	iRIC::rotateVector270(headCenterToRight);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - centerLine[0].x(), x[1] - centerLine[0].y());
		if (QPointF::dotProduct(headCenterToRight, tmp) < 0) {
			discardHead = true;
		}
	}

	// centerToRight vector at middle
	int count = centerLine.count();
	int middleCount = (count - 1) / 2;
	QPointF middleCenterToRight = centerLine[middleCount + 1] - centerLine[middleCount];
	iRIC::rotateVector270(middleCenterToRight);

	int middleEdge = m_lowWaterChannelPolygon->getEdgeThatIntersect(QLineF(centerLine[middleCount], centerLine[middleCount] + QPointF(middleCenterToRight.x(), middleCenterToRight.y())));

	// centerToRight vector at tail
	QPointF tailCenterToRight = centerLine[count - 1] - centerLine[count - 2];
	iRIC::rotateVector(tailCenterToRight, 280);

	int tailEdge   = m_lowWaterChannelPolygon->getEdgeThatIntersect(QLineF(centerLine[count - 1], centerLine[count - 1] + QPointF(tailCenterToRight.x(), tailCenterToRight.y())));

	bool discardTail = false;
	cell = m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(tailEdge);
	tailCenterToRight = centerLine[count - 1] - centerLine[count - 2];
	iRIC::rotateVector270(tailCenterToRight);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - centerLine[count - 1].x(), x[1] - centerLine[count - 1].y());
		if (QPointF::dotProduct(tailCenterToRight, tmp) < 0) {
			discardTail = true;
		}
	}

	QPointF hintVec = centerLine[count - 1] - centerLine[0];
	return getLowWaterEdgeSpline(headEdge, middleEdge, tailEdge, discardHead, discardTail, QVector2D(hintVec.x(), hintVec.y()));
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getLowWaterEdgeSpline(int headEdge, int middleEdge, int tailEdge, bool discardHead, bool discardTail, const QVector2D& hintVec)
{
	int dir;
	if (headEdge <= tailEdge) {
		if (headEdge <= middleEdge && middleEdge <= tailEdge) {
			dir = 1;
		} else {
			dir = - 1;
		}
	} else {
		if (tailEdge <= middleEdge && middleEdge <= headEdge) {
			dir = - 1;
		} else {
			dir = 1;
		}
	}
	int numEdges = m_lowWaterChannelPolygon->getVtkPolygon()->GetNumberOfEdges();
	if (discardHead) {
		headEdge += dir;
		if (headEdge == -1) {headEdge = numEdges - 1;}
		if (headEdge == numEdges) {headEdge = 0;}
	}
	if (discardTail) {
		tailEdge -= dir;
		if (tailEdge == -1) {tailEdge = numEdges - 1;}
		if (tailEdge == numEdges) {tailEdge = 0;}
	}
	if (headEdge == middleEdge && middleEdge == tailEdge) {
		// there is only one edge. we use hintVec to set dir.
		vtkPoints* tmpPoints = m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(headEdge)->GetPoints();
		double v[3];
		tmpPoints->GetPoint(0, v);
		QVector2D p0(v[0], v[1]);
		tmpPoints->GetPoint(1, v);
		QVector2D p1(v[0], v[1]);
		if (QVector2D::dotProduct(p1 - p0, hintVec) > 0) {
			dir = 1;
		} else {
			dir = -1;
		}
	}
	int edgeId = headEdge;
	QVector<QPointF> points;
	while (edgeId != tailEdge) {
		double v[3];
		if (dir == 1) {
			m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(0, v);
		} else {
			m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		}
		points.append(QPointF(v[0], v[1]));

		edgeId = edgeId + dir;
		if (edgeId == -1) {edgeId = numEdges - 1;}
		if (edgeId == numEdges) {edgeId = 0;}
	}
	double v[3];
	if (dir == 1) {
		m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(0, v);
		points.append(QPointF(v[0], v[1]));
		m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		points.append(QPointF(v[0], v[1]));
	} else {
		m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		points.append(QPointF(v[0], v[1]));
		m_lowWaterChannelPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(0, v);
		points.append(QPointF(v[0], v[1]));
	}
	GridCreatingConditionCompoundChannelSpline ret;
	ret.setPoints(points);
	return ret;
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getLeftEdge(const QVector<QPointF>& leftBank)
{
	// centerToLeft vector at head
	QPointF headCenterToLeft = leftBank[1] - leftBank[0];
	iRIC::rotateVector(headCenterToLeft, 110);

	int headEdge   = m_gridRegionPolygon->getEdgeThatIntersect(QLineF(leftBank[0], leftBank[0] + QPointF(headCenterToLeft.x(), headCenterToLeft.y())));
	bool discardHead = false;
	vtkCell* cell = m_gridRegionPolygon->getVtkPolygon()->GetEdge(headEdge);
	headCenterToLeft = leftBank[1] - leftBank[0];
	iRIC::rotateVector90(headCenterToLeft);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - leftBank[0].x(), x[1] - leftBank[0].y());
		if (QPointF::dotProduct(headCenterToLeft, tmp) < 0) {
			discardHead = true;
		}
	}

	// centerToLeft vector at middle
	int count = leftBank.count();
	int middleCount = (count - 1) / 2;
	QPointF middleCenterToLeft = leftBank[middleCount + 1] - leftBank[middleCount];
	iRIC::rotateVector90(middleCenterToLeft);

	int middleEdge = m_gridRegionPolygon->getEdgeThatIntersect(QLineF(leftBank[middleCount], leftBank[middleCount] + QPointF(middleCenterToLeft.x(), middleCenterToLeft.y())));

	// centerToLeft vector at tail
	QPointF tailCenterToLeft = leftBank[count - 1] - leftBank[count - 2];
	iRIC::rotateVector(tailCenterToLeft, 70);

	int tailEdge   = m_gridRegionPolygon->getEdgeThatIntersect(QLineF(leftBank[count - 1], leftBank[count - 1] + QPointF(tailCenterToLeft.x(), tailCenterToLeft.y())));
	bool discardTail = false;
	cell = m_gridRegionPolygon->getVtkPolygon()->GetEdge(tailEdge);
	tailCenterToLeft = leftBank[count - 1] - leftBank[count - 2];
	iRIC::rotateVector90(tailCenterToLeft);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - leftBank[count - 1].x(), x[1] - leftBank[count - 1].y());
		if (QPointF::dotProduct(tailCenterToLeft, tmp) < 0) {
			discardTail = true;
		}
	}
	QPointF hintVec = leftBank[count - 1] - leftBank[0];
	return getRegionEdgeSpline(headEdge, middleEdge, tailEdge, discardHead, discardTail, QVector2D(hintVec.x(), hintVec.y()));
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getRightEdge(const QVector<QPointF>& rightBank)
{
	QPointF headCenterToRight = rightBank[1] - rightBank[0];
	iRIC::rotateVector(headCenterToRight, 250);

	int headEdge   = m_gridRegionPolygon->getEdgeThatIntersect(QLineF(rightBank[0], rightBank[0] + QPointF(headCenterToRight.x(), headCenterToRight.y())));
	bool discardHead = false;
	vtkCell* cell = m_gridRegionPolygon->getVtkPolygon()->GetEdge(headEdge);
	headCenterToRight = rightBank[1] - rightBank[0];
	iRIC::rotateVector270(headCenterToRight);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - rightBank[0].x(), x[1] - rightBank[0].y());
		if (QPointF::dotProduct(headCenterToRight, tmp) < 0) {
			discardHead = true;
		}
	}

	// centerToRight vector at middle
	int count = rightBank.count();
	int middleCount = (count - 1) / 2;
	QPointF middleCenterToRight = rightBank[middleCount + 1] - rightBank[middleCount];
	iRIC::rotateVector270(middleCenterToRight);

	int middleEdge = m_gridRegionPolygon->getEdgeThatIntersect(QLineF(rightBank[middleCount], rightBank[middleCount] + QPointF(middleCenterToRight.x(), middleCenterToRight.y())));

	// centerToRight vector at tail
	QPointF tailCenterToRight = rightBank[count - 1] - rightBank[count - 2];
	iRIC::rotateVector(tailCenterToRight, 290);

	int tailEdge   = m_gridRegionPolygon->getEdgeThatIntersect(QLineF(rightBank[count - 1], rightBank[count - 1] + QPointF(tailCenterToRight.x(), tailCenterToRight.y())));
	bool discardTail = false;
	cell = m_gridRegionPolygon->getVtkPolygon()->GetEdge(tailEdge);
	tailCenterToRight = rightBank[count - 1] - rightBank[count - 2];
	iRIC::rotateVector270(tailCenterToRight);
	for (vtkIdType vId = 0; vId < cell->GetNumberOfPoints(); ++vId) {
		double x[3];
		cell->GetPoints()->GetPoint(vId, x);
		QPointF tmp(x[0] - rightBank[count - 1].x(), x[1] - rightBank[count - 1].y());
		if (QPointF::dotProduct(tailCenterToRight, tmp) < 0) {
			discardTail = true;
		}
	}

	QPointF hintVec = rightBank[count - 1] - rightBank[0];
	return getRegionEdgeSpline(headEdge, middleEdge, tailEdge, discardHead, discardTail, QVector2D(hintVec.x(), hintVec.y()));
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getRegionEdgeSpline(int splitEdge)
{
	int numEdges = m_gridRegionPolygon->getVtkPolygon()->GetNumberOfEdges();
	int edgeId = splitEdge;
	QVector<QPointF> points;

	double v[3];
	m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
	points.append(QPointF(v[0], v[1]));
	++ edgeId;
	if (edgeId == numEdges) {edgeId = 0;}

	while (edgeId != splitEdge) {
		m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		points.append(QPointF(v[0], v[1]));
		++ edgeId;
		if (edgeId == numEdges) {edgeId = 0;}
	}

	GridCreatingConditionCompoundChannelSpline ret;
	ret.setPoints(points);
	return ret;
}

GridCreatingConditionCompoundChannelSpline GridCreatingConditionCompoundChannel::getRegionEdgeSpline(int headEdge, int middleEdge, int tailEdge, bool discardHead, bool discardTail, const QVector2D& hintVec)
{
	int dir;
	if (headEdge <= tailEdge) {
		if (headEdge <= middleEdge && middleEdge <= tailEdge) {
			dir = 1;
		} else {
			dir = - 1;
		}
	} else {
		if (tailEdge <= middleEdge && middleEdge <= headEdge) {
			dir = - 1;
		} else {
			dir = 1;
		}
	}
	int numEdges = m_gridRegionPolygon->getVtkPolygon()->GetNumberOfEdges();
	if (discardHead) {
		headEdge += dir;
		if (headEdge == -1) {headEdge = numEdges - 1;}
		if (headEdge == numEdges) {headEdge = 0;}
	}
	if (discardTail) {
		tailEdge -= dir;
		if (tailEdge == -1) {tailEdge = numEdges - 1;}
		if (tailEdge == numEdges) {tailEdge = 0;}
	}
	if (headEdge == middleEdge && middleEdge == tailEdge) {
		// there is only one edge. we use hintVec to set dir.
		vtkPoints* tmpPoints = m_gridRegionPolygon->getVtkPolygon()->GetEdge(headEdge)->GetPoints();
		double v[3];
		tmpPoints->GetPoint(0, v);
		QVector2D p0(v[0], v[1]);
		tmpPoints->GetPoint(1, v);
		QVector2D p1(v[0], v[1]);
		if (QVector2D::dotProduct(p1 - p0, hintVec) > 0) {
			dir = 1;
		} else {
			dir = -1;
		}
	}
	int edgeId = headEdge;
	QVector<QPointF> points;
	while (edgeId != tailEdge) {
		double v[3];
		if (dir == 1) {
			m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(0, v);
		} else {
			m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		}
		points.append(QPointF(v[0], v[1]));

		edgeId = edgeId + dir;
		if (edgeId == -1) {edgeId = numEdges - 1;}
		if (edgeId == numEdges) {edgeId = 0;}
	}
	double v[3];
	if (dir == 1) {
		m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(0, v);
		points.append(QPointF(v[0], v[1]));
		m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		points.append(QPointF(v[0], v[1]));
	} else {
		m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(1, v);
		points.append(QPointF(v[0], v[1]));
		m_gridRegionPolygon->getVtkPolygon()->GetEdge(edgeId)->GetPoints()->GetPoint(0, v);
		points.append(QPointF(v[0], v[1]));
	}
	GridCreatingConditionCompoundChannelSpline ret;
	ret.setPoints(points);
	return ret;
}

void GridCreatingConditionCompoundChannel::update2Ds()
{}
