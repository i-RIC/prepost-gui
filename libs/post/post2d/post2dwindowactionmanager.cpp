#include "post2dwindow.h"
#include "post2dwindowactionmanager.h"
#include "post2dwindowdatamodel.h"

#include <guibase/irictoolbar.h>
#include <guicore/base/iricmainwindowinterface.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

Post2dWindowActionManager::Post2dWindowActionManager(Post2dWindow* parent)
	: QObject(parent)
{
	m_post2dWindow = parent;
	init();
}

void Post2dWindowActionManager::init()
{
	setupDrawMenu();
	setupMeasuredDataMenu();
}

void Post2dWindowActionManager::setupDrawMenu()
{
	m_drawMenu = new QMenu(tr("&Draw"), m_post2dWindow);

	drawGridAction = new QAction(tr("&Grid Shape..."), this);
	m_drawMenu->addAction(drawGridAction);

	drawContourAction = new QAction(tr("&Contours (node)..."), this);
	m_drawMenu->addAction(drawContourAction);

	drawArrowAction = new QAction(tr("&Arrows..."), this);
	m_drawMenu->addAction(drawArrowAction);

	drawStreamlineAction = new QAction(tr("&Streamlines..."), this);
	m_drawMenu->addAction(drawStreamlineAction);

	drawParticleAction = new QAction(tr("&Particles..."), this);
	m_drawMenu->addAction(drawParticleAction);

	drawPolygonAction = new QAction(tr("P&olygons..."), this);
	m_drawMenu->addAction(drawPolygonAction);

	drawCellFlagAction = new QAction(tr("&Cell Attributes..."), this);
	m_drawMenu->addAction(drawCellFlagAction);

	drawCellScalarAction = new QAction(tr("Contours (ce&ll center)..."), this);
	m_drawMenu->addAction(drawCellScalarAction);

	m_drawMenu->addSeparator();

	drawTitleAction = new QAction(tr("&Title..."), this);
	m_drawMenu->addAction(drawTitleAction);

	drawTimeAction = new QAction(tr("Ti&me..."), this);
	m_drawMenu->addAction(drawTimeAction);
}

void Post2dWindowActionManager::setupMeasuredDataMenu()
{
	m_measuredDataMenu = new QMenu(tr("&Measured Data"), m_post2dWindow);

	measuredDataPointAction = new QAction(tr("&Scalar..."), this);
	m_measuredDataMenu->addAction(measuredDataPointAction);

	measuredDataVectorAction = new QAction(tr("&Arrows..."), this);
	m_measuredDataMenu->addAction(measuredDataVectorAction);

	m_measuredDataMenu->addSeparator();

	measuredDataImportAction = new QAction(QIcon(":/libs/guibase/images/iconImport.png"),tr("&Import..."), this);
	m_measuredDataMenu->addAction(measuredDataImportAction);
}

void Post2dWindowActionManager::connectWithDataModel()
{
	connect(drawGridAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(gridShapeSetting()));
	connect(drawContourAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(contourSetting()));
	connect(drawArrowAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(vectorSetting()));
	connect(drawStreamlineAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(streamlineSetting()));
	connect(drawParticleAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(particleSetting()));
	connect(drawPolygonAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(polygonSetting()));
	connect(drawCellFlagAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(cellFlagSetting()));
	connect(drawCellScalarAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(cellScalarSetting()));
	connect(drawTitleAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(titleSetting()));
	connect(drawTimeAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(timeSetting()));

	connect(measuredDataPointAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(measuredDataPointSetting()));
	connect(measuredDataVectorAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(measuredDataVectorSetting()));
	connect(measuredDataImportAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel->iricMainWindow(), SLOT(importMeasuredData()));
}
