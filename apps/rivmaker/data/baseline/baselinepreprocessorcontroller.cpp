#include "baselinepreprocessorcontroller.h"
#include "../base/model.h"
#include "../base/view.h"
#include "../crosssections/crosssectionspreprocessorcontroller.h"
#include "../project/project.h"
#include "../../misc/geometryutil.h"

#include "private/baselinepreprocessorcontroller_impl.h"

#include <QIcon>
#include <QKeyEvent>

BaseLinePreProcessorController::Impl::Impl() :
	m_mode {Mode::BeforeDefining},
	m_movingPointIndex {0},
	m_addingEdgeIndex {0},
	m_removingPointIndex {0},
	m_addVertexAction {new QAction(QIcon(":/images/iconAddPolygonVertex.png"), tr("&Add Vertex"), nullptr)},
	m_removeVertexAction {new QAction(QIcon(":/images/iconRemovePolygonVertex.png"), tr("&Remove Vertex"), nullptr)},
	m_editCoordinatesAction {new QAction(tr("&Edit Coordinates..."), nullptr)},
	m_reverseDirectionAction {new QAction(tr("Reverse &Direction"), nullptr)},
	m_deleteAction {new QAction(QIcon(":/images/iconDelete.png"), tr("Delete"), nullptr)}
{}

BaseLinePreProcessorController::Impl::~Impl()
{
	delete m_addVertexAction;
	delete m_removeVertexAction;
	delete m_editCoordinatesAction;
	delete m_reverseDirectionAction;
	delete m_deleteAction;
}

// public interfaces

BaseLinePreProcessorController::BaseLinePreProcessorController(Model* model, BaseLine* item) :
	DataItemController {model, item},
	impl {new Impl {}}
{
	objectBrowserRightClickMenu().addAction(impl->m_deleteAction);

	viewRightClickMenu().addAction(impl->m_addVertexAction);
	viewRightClickMenu().addAction(impl->m_removeVertexAction);
	viewRightClickMenu().addSeparator();
	viewRightClickMenu().addAction(impl->m_editCoordinatesAction);
	viewRightClickMenu().addAction(impl->m_reverseDirectionAction);
	viewRightClickMenu().addSeparator();
	viewRightClickMenu().addAction(impl->m_deleteAction);

	connect(impl->m_addVertexAction, SIGNAL(triggered()), this, SLOT(addVertex()));
	connect(impl->m_removeVertexAction, SIGNAL(triggered()), this, SLOT(removeVertex()));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(clear()));
}

BaseLinePreProcessorController::~BaseLinePreProcessorController()
{
	delete impl;
}

void BaseLinePreProcessorController::keyPressEvent(QKeyEvent* event, View*)
{
	if (impl->m_mode != Impl::Mode::Defining) {return;}
	if (event->key() != Qt::Key_Return) {return;}

	finishDefining();
}

void BaseLinePreProcessorController::mouseDoubleClickEvent(QMouseEvent*, View*)
{
	if (impl->m_mode != Impl::Mode::Defining) {return;}

	finishDefining();
}

void BaseLinePreProcessorController::mouseMoveEvent(QMouseEvent* event, View* v)
{
	auto baseLine = dynamic_cast<BaseLine*> (item());
	QPointF p = v->rconv(QPointF(event->x(), event->y()));
	std::vector<QPointF> polyline = baseLine->polyLine();

	if (impl->m_mode == Impl::Mode::Defining) {
		polyline.pop_back();
		polyline.push_back(p);
		baseLine->setPolyLine(polyline);
		updateView();
	} else if (impl->m_mode == Impl::Mode::Normal || impl->m_mode == Impl::Mode::MovePointPrepare) {
		impl->m_mode = Impl::Mode::Normal;
		for (int i = 0; i < polyline.size(); ++i) {
			QPointF p2 = polyline.at(i);
			QPointF p3 = v->conv(p2);
			if (View::isNear(QPointF(event->pos()), p3)) {
				impl->m_mode = Impl::Mode::MovePointPrepare;
				impl->m_movingPointIndex = i;
			}
		}
		updateMouseCursor(v);
	} else if (impl->m_mode == Impl::Mode::AddPointNotPossible || impl->m_mode == Impl::Mode::AddPointPrepare) {
		impl->m_mode = Impl::Mode::AddPointNotPossible;
		for (int i = 0; i < polyline.size() - 1; ++i) {
			QPointF p1 = polyline.at(i);
			QPointF p2 = polyline.at(i + 1);
			QPointF np = GeometryUtil::nearestPoint(p1, p2, p);
			QPointF np2 = v->conv(np);
			if (View::isNear(QPointF(event->pos()), np2)) {
				impl->m_mode = Impl::Mode::AddPointPrepare;
				impl->m_addingEdgeIndex = i;
				break;
			}
		}
	} else if (impl->m_mode == Impl::Mode::RemovePointNotPossible || impl->m_mode == Impl::Mode::RemovePointPrepare) {
		impl->m_mode = Impl::Mode::RemovePointNotPossible;
		for (int i = 0; i < polyline.size(); ++i) {
			QPointF p2 = polyline.at(i);
			QPointF p3 = v->conv(p2);
			if (View::isNear(QPointF(event->pos()), p3)) {
				impl->m_mode = Impl::Mode::RemovePointPrepare;
				impl->m_removingPointIndex = i;
			}
		}
		updateMouseCursor(v);
	} else if (impl->m_mode == Impl::Mode::MovePoint) {
		polyline[impl->m_movingPointIndex] = p;
		baseLine->setPolyLine(polyline);
		updateView();
	}
}

void BaseLinePreProcessorController::mousePressEvent(QMouseEvent* event, View* v)
{
	auto baseLine = dynamic_cast<BaseLine*> (item());
	QPointF p = v->rconv(QPointF(event->x(), event->y()));
	std::vector<QPointF> polyline = baseLine->polyLine();

	if (impl->m_mode == Impl::Mode::BeforeDefining) {
		polyline.push_back(p);
		polyline.push_back(p);
		baseLine->setPolyLine(polyline);
		impl->m_mode = Impl::Mode::Defining;
	} else if (impl->m_mode == Impl::Mode::Defining) {
		polyline.push_back(p);
		baseLine->setPolyLine(polyline);
	} else if (impl->m_mode == Impl::Mode::MovePointPrepare) {
		impl->m_mode = Impl::Mode::MovePoint;
	} else if (impl->m_mode == Impl::Mode::AddPointPrepare) {
		auto it = polyline.begin() + (impl->m_addingEdgeIndex + 1);
		polyline.insert(it, p);
		baseLine->setPolyLine(polyline);
		impl->m_movingPointIndex = impl->m_addingEdgeIndex + 1;
		impl->m_mode = Impl::Mode::AddPoint;
		updateView();
	} else if (impl->m_mode == Impl::Mode::RemovePointPrepare) {
		auto it = polyline.begin() + impl->m_removingPointIndex;
		polyline.erase(it);
		baseLine->setPolyLine(polyline);
		finishDefining();
	}
	updateMouseCursor(v);
}

void BaseLinePreProcessorController::mouseReleaseEvent(QMouseEvent*, View* v)
{
	if (impl->m_mode == Impl::Mode::MovePoint || impl->m_mode == Impl::Mode::AddPoint) {
		finishDefining();
	}

	updateMouseCursor(v);
}

void BaseLinePreProcessorController::addVertex()
{
	if (impl->m_mode == Impl::Mode::Normal) {
		impl->m_mode = Impl::Mode::AddPointNotPossible;
	} else if (impl->m_mode == Impl::Mode::AddPointNotPossible) {
		impl->m_mode = Impl::Mode::Normal;
	}
}

void BaseLinePreProcessorController::removeVertex()
{
	if (impl->m_mode == Impl::Mode::Normal) {
		impl->m_mode = Impl::Mode::RemovePointNotPossible;
	} else if (impl->m_mode == Impl::Mode::RemovePointNotPossible) {
		impl->m_mode = Impl::Mode::Normal;
	}
}

void BaseLinePreProcessorController::clear()
{
	auto baseLine = dynamic_cast<BaseLine*> (item());

	std::vector<QPointF> emptyLine;
	baseLine->setPolyLine(emptyLine);

	impl->m_mode = Impl::Mode::BeforeDefining;

	item()->project()->emitUpdated();

	updateView();
}

void BaseLinePreProcessorController::finishDefining()
{
	auto baseLine = dynamic_cast<BaseLine*> (item());
	if (baseLine->polyLine().size() < 2) {return;}

	impl->m_mode = Impl::Mode::Normal;

	auto p = item()->project();
	bool sorted = p->sortCrossSectionsIfPossible();
	if (! sorted) {return;}

	auto csCtrl = dynamic_cast<CrossSectionsPreProcessorController*> (model()->dataItemController(&(p->crossSections())));
	csCtrl->rebuildStandardItemsAndViews();

	p->emitUpdated();

	updateView();
}

void BaseLinePreProcessorController::updateMouseCursor(View* v)
{
	switch (impl->m_mode) {
	case Impl::Mode::MovePointPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case Impl::Mode::MovePoint:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	default:
		v->setCursor(Qt::ArrowCursor);
	}
}
