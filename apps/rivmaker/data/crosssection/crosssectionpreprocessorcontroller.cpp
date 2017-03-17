#include "crosssectionpreprocessorcontroller.h"
#include "../base/model.h"
#include "../base/view.h"
#include "../crosssections/crosssectionspreprocessorcontroller.h"
#include "../project/project.h"

#include <QMouseEvent>

CrossSectionPreProcessorController::CrossSectionPreProcessorController(Model* model, CrossSection* item) :
	DataItemController {model, item},
	impl {new Impl {}}
{
	impl->m_mode = Impl::Mode::BeforeDefining;
}

CrossSectionPreProcessorController::~CrossSectionPreProcessorController()
{
	delete impl;
}

void CrossSectionPreProcessorController::mouseMoveEvent(QMouseEvent* event, View* v)
{
	auto cs = dynamic_cast<CrossSection*> (item());
	QPointF p = v->rconv(QPointF(event->x(), event->y()));

	if (impl->m_mode == Impl::Mode::Defining){
		cs->setPoint2(p);
		updateView();
	} else if (impl->m_mode == Impl::Mode::Normal || impl->m_mode == Impl::Mode::MovePointPrepare) {
		impl->m_mode = Impl::Mode::Normal;
		for (int i = 0; i < 2; ++i) {
			QPointF p2 = cs->point(i);
			QPointF p3 = v->conv(p2);
			if (View::isNear(QPointF(event->pos()), p3)) {
				impl->m_mode = Impl::Mode::MovePointPrepare;
				impl->m_movingPointIndex = i;
			}
		}
		updateMouseCursor(v);
	} else if (impl->m_mode == Impl::Mode::MovePoint) {
		cs->setPoint(impl->m_movingPointIndex, p);
		updateView();
	}
}

void CrossSectionPreProcessorController::mousePressEvent(QMouseEvent* event, View* v)
{
	auto cs = dynamic_cast<CrossSection*> (item());
	QPointF p = v->rconv(QPointF(event->x(), event->y()));

	if (impl->m_mode == Impl::Mode::BeforeDefining) {
		cs->setPoint1(p);
		cs->setPoint2(p);
		impl->m_mode = Impl::Mode::Defining;

		updateView();
	} else if (impl->m_mode == Impl::Mode::MovePointPrepare) {
		impl->m_mode = Impl::Mode::MovePoint;
	}
	updateMouseCursor(v);
}

void CrossSectionPreProcessorController::mouseReleaseEvent(QMouseEvent*, View* v)
{
	finishDefining();
	updateMouseCursor(v);
}

void CrossSectionPreProcessorController::finishDefining()
{
	impl->m_mode = Impl::Mode::Normal;

	auto p = item()->project();
	bool sorted = p->sortCrossSectionsIfPossible();
	if (! sorted) {return;}

	auto csCtrl = dynamic_cast<CrossSectionsPreProcessorController*> (model()->dataItemController(&(p->crossSections())));
	csCtrl->rebuildStandardItemsAndViews();

	model()->select(item());

	updateView();
}

void CrossSectionPreProcessorController::updateMouseCursor(View* v)
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
