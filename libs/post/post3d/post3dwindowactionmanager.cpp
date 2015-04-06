#include "post3dwindow.h"
#include "post3dwindowactionmanager.h"
#include "post3dwindowdatamodel.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

Post3dWindowActionManager::Post3dWindowActionManager(Post3dWindow* parent)
	: QObject(parent)
{
	m_post3dWindow = parent;
	init();
}

void Post3dWindowActionManager::init()
{
	setupDrawMenu();
}

void Post3dWindowActionManager::setupDrawMenu()
{
	m_drawMenu = new QMenu(tr("&Draw"), m_post3dWindow);

	drawGridAction = new QAction(tr("&Grid Shape..."), m_drawMenu);
	m_drawMenu->addAction(drawGridAction);

	drawContourAction = new QAction(tr("&Contours..."), m_drawMenu);
	m_drawMenu->addAction(drawContourAction);

	drawIsoSurfaceAction = new QAction(tr("&Iso Surface"), m_drawMenu);
	m_drawMenu->addAction(drawIsoSurfaceAction);

	drawVectorAction = new QAction(tr("&Arrows..."), m_drawMenu);
	m_drawMenu->addAction(drawVectorAction);

	drawStreamlineAction = new QAction(tr("&Streamlines..."), m_drawMenu);
	m_drawMenu->addAction(drawStreamlineAction);

	drawParticleTraceAction = new QAction(tr("&Particles..."), m_drawMenu);
	m_drawMenu->addAction(drawParticleTraceAction);

	m_drawMenu->addSeparator();

	drawTitleAction = new QAction(tr("&Title..."), m_drawMenu);
	m_drawMenu->addAction(drawTitleAction);

	drawTimeAction = new QAction(tr("Ti&me..."), m_drawMenu);
	m_drawMenu->addAction(drawTimeAction);

}

void Post3dWindowActionManager::connectWithDataModel()
{
	connect(drawGridAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(gridShapeSetting()));
	connect(drawContourAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(contourSetting()));
	connect(drawVectorAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(arrowSetting()));
	connect(drawStreamlineAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(streamlineSetting()));
	connect(drawParticleTraceAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(particleSetting()));
	connect(drawIsoSurfaceAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(isosurfaceSetting()));
	connect(drawTitleAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(titleSetting()));
	connect(drawTimeAction, SIGNAL(triggered()), m_post3dWindow->m_dataModel, SLOT(timeSetting()));
}

