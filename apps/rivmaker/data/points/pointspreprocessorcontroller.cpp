#include "pointspreprocessorcontroller.h"

#include "private/pointspreprocessorcontroller_impl.h"
#include "../base/view.h"
#include "../project/project.h"
#include "../../geom/geometrypoint.h"
#include "../../io/points/pointsimporter.h"
#include "../../window/preprocessor/preprocessormodel.h"

#include <QAction>
#include <QMenu>
#include <QMouseEvent>

PointsPreProcessorController::Impl::Impl() :
	m_importAction {new QAction(QIcon(":/images/iconImport.png"), PointsPreProcessorController::tr("&Import..."), nullptr)},
	m_deleteAction {new QAction(QIcon(":/images/iconDeleteItem.png"), PointsPreProcessorController::tr("&Delete"), nullptr)}
{}

PointsPreProcessorController::Impl::~Impl()
{
	delete m_importAction;
	delete m_deleteAction;
}

PointsPreProcessorController::PointsPreProcessorController(Model* model, Points* item) :
	DataItemController {model, item},
	impl {new Impl {}}
{
	connect(impl->m_importAction, SIGNAL(triggered()), this, SLOT(importData()));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(clearData()));
}

PointsPreProcessorController::~PointsPreProcessorController()
{
	delete impl;
}

void PointsPreProcessorController::mouseMoveEvent(QMouseEvent* event, View* view)
{
	QPointF mousePos = event->pos();
	auto m = dynamic_cast<PreProcessorModel*> (model());

	auto points = dynamic_cast<Points*> (item());
	for (GeometryPoint* p : points->points()){
		auto p_pos = view->conv(QPointF(p->x(), p->y()));
		if (View::isNear(mousePos, p_pos)) {
			m->emitValueChange(p->z());
			return;
		}
	}
	m->emitValueClear();
}

void PointsPreProcessorController::setupObjectBrowserRightClickMenu(QMenu* menu)
{
	menu->addAction(impl->m_importAction);
	menu->addSeparator();
	menu->addAction(impl->m_deleteAction);
}

void PointsPreProcessorController::importData()
{
	auto points = dynamic_cast<Points*> (item());

	std::vector<GeometryPoint*> newPoints;
	QPointF offset = points->project()->offset();

	bool ok = PointsImporter::importData(&newPoints, &offset, view());
	if (! ok) {return;}

	points->clearPoints();
	points->setPoints(newPoints);
	points->project()->setOffset(offset);

	points->project()->emitUpdated();

	view()->fit();
}

void PointsPreProcessorController::clearData()
{
	auto points = dynamic_cast<Points*> (item());
	points->clearPoints();

	points->project()->emitUpdated();
}
