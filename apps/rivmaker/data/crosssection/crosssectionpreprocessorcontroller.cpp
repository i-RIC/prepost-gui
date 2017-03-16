#include "crosssectionpreprocessorcontroller.h"
#include "../base/view.h"

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
	if (impl->m_mode != Impl::Mode::Defining) {return;}

	auto cs = dynamic_cast<CrossSection*> (item());

	QPointF p = v->rconv(QPointF(event->x(), event->y()));
	cs->setPoint2(p);

	updateView();
}

void CrossSectionPreProcessorController::mousePressEvent(QMouseEvent* event, View* v)
{
	if (impl->m_mode == Impl::Mode::Normal) {return;}

	auto cs = dynamic_cast<CrossSection*> (item());

	QPointF p = v->rconv(QPointF(event->x(), event->y()));
	cs->setPoint1(p);
	cs->setPoint2(p);
	impl->m_mode = Impl::Mode::Defining;

	updateView();
}

void CrossSectionPreProcessorController::mouseReleaseEvent(QMouseEvent*, View*)
{
	impl->m_mode = Impl::Mode::Normal;
}
