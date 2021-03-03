#include "crosssectionpreprocessorcontroller.h"
#include "../base/model.h"
#include "../base/view.h"
#include "../crosssections/crosssectionspreprocessorcontroller.h"
#include "../project/project.h"
#include "../../dialogs/coordinateseditdialog.h"
#include "../../window/preprocessor/preprocessormodel.h"

#include "private/crosssectionpreprocessorcontroller_impl.h"

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegExp>

CrossSectionPreProcessorController::Impl::Impl() :
	m_mode {Mode::BeforeDefining},
	m_movingPointIndex {0},
	m_editCoordinatesAction {new QAction(CrossSectionPreProcessorController::tr("&Edit Coordinates..."), nullptr)}
{}

CrossSectionPreProcessorController::Impl::~Impl()
{
	delete m_editCoordinatesAction;
}

CrossSectionPreProcessorController::CrossSectionPreProcessorController(Model* model, CrossSection* item) :
	DataItemController {model, item},
	impl {new Impl {}}
{
	connect(impl->m_editCoordinatesAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));

	if (item->isDefined()) {
		impl->m_mode = Impl::Mode::Normal;
	}
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
	} else if (impl->m_mode == Impl::Mode::MovePoint) {
		cs->setPoint(impl->m_movingPointIndex, p);
		cs->project()->setModified();
		updateView();
	}
	updateMouseCursor(v);
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
	if (impl->m_mode == Impl::Mode::Defining || impl->m_mode == Impl::Mode::MovePoint) {
		finishDefining();
	}
	updateMouseCursor(v);
}

void CrossSectionPreProcessorController::setupViewRightClickMenu(QMenu* menu)
{
	menu->addAction(impl->m_editCoordinatesAction);
}

void CrossSectionPreProcessorController::handleStandardItemChange(QStandardItem* sitem)
{
	QString newName = sitem->text();
	QRegExp xnum("XS{0,1}[0-9]+");
	QRegExp num("[0-9\\.]+");
	bool isDouble;

	newName.toDouble(&isDouble);

	auto cs = dynamic_cast<CrossSection*> (item());
	if (xnum.exactMatch(newName)) {
		cs->setName(newName);
		cs->project()->setModified();
	} else if (num.exactMatch(newName) && isDouble) {
		cs->setName(newName);
		cs->project()->setModified();
	} else {
		QMessageBox::warning(view(), tr("Warning"), tr("Invalid name for cross section. The name should be \"X1\", \"X2\", ...　or \"XS1\", \"XS2\", ..."));
		sitem->setText(cs->name());
	}
}

void CrossSectionPreProcessorController::editCoordinates()
{
	auto cs = dynamic_cast<CrossSection*> (item());
	if (! cs->isDefined()) {
		QMessageBox::warning(view(), tr("Warning"), tr("Cross Section is not defined yet."));
		return;
	}

	CoordinatesEditDialog dialog(view());
	dialog.setOffset(item()->project()->offset());
	dialog.setTarget(cs);
	dialog.exec();

	finishDefining();
}

void CrossSectionPreProcessorController::finishDefining()
{
	impl->m_mode = Impl::Mode::Normal;

	auto preModel = dynamic_cast<PreProcessorModel*> (model());
	preModel->updateCrossSections();

	updateView();
}

void CrossSectionPreProcessorController::updateMouseCursor(View* v)
{
	switch (impl->m_mode) {
	case Impl::Mode::BeforeDefining:
	case Impl::Mode::Defining:
		v->setCursor(Qt::CrossCursor);
		break;
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
