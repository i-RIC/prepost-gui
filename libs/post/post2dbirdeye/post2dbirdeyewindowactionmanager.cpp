#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowactionmanager.h"
#include "post2dbirdeyewindowdatamodel.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

Post2dBirdEyeWindowActionManager::Post2dBirdEyeWindowActionManager(Post2dBirdEyeWindow* parent) :
	QObject(parent),
	m_post2dWindow {parent}
{
	setupDrawMenu();
}

QMenu* Post2dBirdEyeWindowActionManager::drawMenu() const
{
	return m_drawMenu;
}

void Post2dBirdEyeWindowActionManager::setupDrawMenu()
{
	m_drawMenu = new QMenu(tr("&Draw"), m_post2dWindow);

	drawGridAction = new QAction(tr("&Grid Shape..."), this);
	m_drawMenu->addAction(drawGridAction);

	drawContourAction = new QAction(tr("&Contours (node)..."), this);
	m_drawMenu->addAction(drawContourAction);

	drawCellScalarAction = new QAction(tr("Contours (ce&ll center)..."), this);
	m_drawMenu->addAction(drawCellScalarAction);

	m_drawMenu->addSeparator();

	drawTitleAction = new QAction(tr("&Title..."), this);
	m_drawMenu->addAction(drawTitleAction);

	drawTimeAction = new QAction(tr("Ti&me..."), this);
	m_drawMenu->addAction(drawTimeAction);

	m_drawMenu->addSeparator();

	backgroundColorAction = new QAction(tr("&Background Color..."), m_drawMenu);
	m_drawMenu->addAction(backgroundColorAction);

	editZScaleAction = new QAction(tr("&Z-direction Scale..."), m_drawMenu);
	m_drawMenu->addAction(editZScaleAction);
}

void Post2dBirdEyeWindowActionManager::connectWithDataModel()
{
	connect(drawGridAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(gridShapeSetting()));
	connect(drawContourAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(contourSetting()));
	connect(drawCellScalarAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(cellScalarSetting()));
	connect(drawTitleAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(titleSetting()));
	connect(drawTimeAction, SIGNAL(triggered()), m_post2dWindow->m_dataModel, SLOT(timeSetting()));
	connect(backgroundColorAction, SIGNAL(triggered()), m_post2dWindow, SLOT(editBackgroundColor()));
	connect(editZScaleAction, SIGNAL(triggered()), m_post2dWindow, SLOT(editZScale()));
}
