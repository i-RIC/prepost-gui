#include "baselinepreprocessorcontroller.h"
#include "../base/model.h"
#include "../base/view.h"
#include "../crosssections/crosssectionspreprocessorcontroller.h"
#include "../project/project.h"

#include "private/baselinepreprocessorcontroller_impl.h"

#include <QKeyEvent>

// public interfaces

BaseLinePreProcessorController::BaseLinePreProcessorController(Model* model, BaseLine* item) :
	DataItemController {model, item},
	impl {new Impl {}}
{
	// @todo update Mode when data is imported etc.
	impl->m_mode = Impl::Mode::BeforeDefining;
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
	}
	updateMouseCursor(v);
}

void BaseLinePreProcessorController::mouseReleaseEvent(QMouseEvent*, View* v)
{
	if (impl->m_mode == Impl::Mode::MovePoint) {
		finishDefining();
	}
	updateMouseCursor(v);
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
