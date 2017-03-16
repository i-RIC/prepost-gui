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
	if (impl->m_mode == Impl::Mode::Defining) {
		QPointF p = v->rconv(QPointF(event->x(), event->y()));
		std::vector<QPointF> polyline = baseLine->polyLine();
		polyline.pop_back();
		polyline.push_back(p);
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
	}
}

void BaseLinePreProcessorController::mouseReleaseEvent(QMouseEvent*, View*)
{}

void BaseLinePreProcessorController::finishDefining()
{
	impl->m_mode = Impl::Mode::Normal;

	auto p = item()->project();
	bool sorted = p->sortCrossSectionsIfPossible();
	if (! sorted) {return;}

	auto csCtrl = dynamic_cast<CrossSectionsPreProcessorController*> (model()->dataItemController(&(p->crossSections())));
	csCtrl->rebuildStandardItemsAndViews();

	updateView();
}
