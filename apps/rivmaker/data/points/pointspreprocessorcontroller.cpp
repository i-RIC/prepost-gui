#include "pointspreprocessorcontroller.h"

#include "private/pointspreprocessorcontroller_impl.h"
#include "../base/view.h"
#include "../project/project.h"
#include "../../io/points/pointsimporter.h"

#include <QAction>
#include <QMenu>

PointsPreProcessorController::Impl::Impl() :
	m_importAction {new QAction(QIcon(":/images/iconImport.png"), tr("&Import..."), nullptr)},
	m_deleteAction {new QAction(QIcon(":/images/iconDeleteItem.png"), tr("&Delete"), nullptr)}
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

void PointsPreProcessorController::setupObjectBrowserRightClickMenu(QMenu* menu)
{
	menu->addAction(impl->m_importAction);
	menu->addSeparator();
	menu->addAction(impl->m_deleteAction);
}

void PointsPreProcessorController::importData()
{
	auto points = dynamic_cast<Points*> (item());

	std::vector<QVector3D*> newPoints;
	QPointF offset = points->project()->offset();

	bool ok = PointsImporter::importData(&newPoints, &offset, view());
	if (! ok) {return;}

	points->clearPoints();
	points->setPoints(newPoints);
	points->project()->setOffset(offset);

	points->project()->emitUpdated();
}

void PointsPreProcessorController::clearData()
{
	auto points = dynamic_cast<Points*> (item());
	points->clearPoints();

	points->project()->emitUpdated();
}
