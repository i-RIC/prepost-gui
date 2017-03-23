#include "baselinepreprocessorcontroller.h"
#include "../base/model.h"
#include "../base/view.h"
#include "../crosssections/crosssectionspreprocessorcontroller.h"
#include "../project/project.h"
#include "../../dialogs/coordinateseditdialog.h"
#include "../../io/polyline/polylineimporter.h"
#include "../../io/polyline/polylineexporter.h"
#include "../../misc/geometryutil.h"
#include "../../window/preprocessor/preprocessormodel.h"

#include "private/baselinepreprocessorcontroller_impl.h"

#include <QAction>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>

BaseLinePreProcessorController::Impl::Impl() :
	m_mode {Mode::BeforeDefining},
	m_movingPointIndex {0},
	m_addingEdgeIndex {0},
	m_removingPointIndex {0},
	m_importAction {new QAction(QIcon(":/images/iconImport.png"), tr("&Import"), nullptr)},
	m_exportAction {new QAction(QIcon(":/images/iconExport.png"), tr("&Export"), nullptr)},
	m_addVertexAction {new QAction(QIcon(":/images/iconAddPolygonVertex.png"), tr("&Add Vertex"), nullptr)},
	m_removeVertexAction {new QAction(QIcon(":/images/iconRemovePolygonVertex.png"), tr("&Remove Vertex"), nullptr)},
	m_editCoordinatesAction {new QAction(tr("&Edit Coordinates..."), nullptr)},
	m_reverseDirectionAction {new QAction(tr("Reverse &Direction"), nullptr)},
	m_deleteAction {new QAction(QIcon(":/images/iconDeleteItem.png"), tr("Delete"), nullptr)},
	m_addPixmap {":/images/cursorAdd.png"},
	m_removePixmap {":/images/cursorRemove.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_removeCursor {m_removePixmap, 0, 0}
{}

BaseLinePreProcessorController::Impl::~Impl()
{
	delete m_importAction;
	delete m_exportAction;
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
	connect(impl->m_importAction, SIGNAL(triggered()), this, SLOT(importData()));
	connect(impl->m_exportAction, SIGNAL(triggered()), this, SLOT(exportData()));
	connect(impl->m_addVertexAction, SIGNAL(triggered()), this, SLOT(addVertex()));
	connect(impl->m_removeVertexAction, SIGNAL(triggered()), this, SLOT(removeVertex()));
	connect(impl->m_editCoordinatesAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));
	connect(impl->m_reverseDirectionAction, SIGNAL(triggered()), this, SLOT(reverseDirection()));
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
		updateMouseCursor(v);
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
	} else if (impl->m_mode == Impl::Mode::MovePoint || impl->m_mode == Impl::Mode::AddPoint) {
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

void BaseLinePreProcessorController::updateMode()
{
	auto bl = dynamic_cast<BaseLine*> (item());
	if (bl->polyLine().size() == 0) {
		impl->m_mode = Impl::Mode::BeforeDefining;
	} else {
		impl->m_mode = Impl::Mode::Normal;
	}
}

void BaseLinePreProcessorController::setupObjectBrowserRightClickMenu(QMenu* menu)
{
	menu->addAction(impl->m_importAction);
	menu->addAction(impl->m_exportAction);
	menu->addSeparator();
	menu->addAction(impl->m_deleteAction);
}

void BaseLinePreProcessorController::setupViewRightClickMenu(QMenu* menu)
{
	menu->addAction(impl->m_addVertexAction);
	menu->addAction(impl->m_removeVertexAction);
	menu->addSeparator();
	menu->addAction(impl->m_editCoordinatesAction);
	menu->addAction(impl->m_reverseDirectionAction);
	menu->addSeparator();
	menu->addAction(impl->m_deleteAction);
}

void BaseLinePreProcessorController::importData()
{
	auto bl = dynamic_cast<BaseLine*> (item());

	std::vector<QPointF> line;
	QPointF offset = bl->project()->offset();
	bool ok = PolyLineImporter::importData(&line, &offset, view());
	if (! ok) {return;}

	bl->setPolyLine(line);
	bl->project()->setOffset(offset);
	updateMode();

	updateView();
}

void BaseLinePreProcessorController::exportData()
{
	auto bl = dynamic_cast<BaseLine*> (item());
	if (bl->polyLine().size() == 0) {
		QMessageBox::warning(view(), tr("Warning"), tr("Base line is not defined yet."));
		return;
	}
	PolyLineExporter::exportData(bl->polyLine(), bl->project()->offset(), view());
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

void BaseLinePreProcessorController::editCoordinates()
{
	auto bl = dynamic_cast<BaseLine*> (item());
	if (bl->polyLine().size() < 2) {
		QMessageBox::warning(view(), tr("Warning"), tr("Base line is not defined yet."));
		return;
	}

	CoordinatesEditDialog dialog(view());
	dialog.setOffset(item()->project()->offset());
	dialog.setTarget(bl);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto preModel = dynamic_cast<PreProcessorModel*> (model());
	preModel->updateCrossSections();
}

void BaseLinePreProcessorController::reverseDirection()
{
	auto bl = dynamic_cast<BaseLine*> (item());
	bl->reverseDirection();

	auto preModel = dynamic_cast<PreProcessorModel*> (model());
	preModel->updateCrossSections();

	updateView();
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

	auto preModel = dynamic_cast<PreProcessorModel*> (model());
	preModel->updateCrossSections();

	updateView();
}

void BaseLinePreProcessorController::updateMouseCursor(View* v)
{
	switch (impl->m_mode) {
	case Impl::Mode::MovePointPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case Impl::Mode::MovePoint:
	case Impl::Mode::AddPoint:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	case Impl::Mode::AddPointPrepare:
		v->setCursor(impl->m_addCursor);
		break;
	case Impl::Mode::RemovePointPrepare:
		v->setCursor(impl->m_removeCursor);
		break;
	default:
		v->setCursor(Qt::ArrowCursor);
	}
}
