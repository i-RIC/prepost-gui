#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowactionmanager.h"
//#include "gridbirdeyewindowdatamodel.h"

#include <QAction>
#include <QMenu>

GridBirdEyeWindowActionManager::GridBirdEyeWindowActionManager(GridBirdEyeWindow* parent)
	: QObject(parent)
{
	m_gridBirdEyeWindow = parent;
	init();
}

void GridBirdEyeWindowActionManager::init()
{
	setupDrawMenu();
}

void GridBirdEyeWindowActionManager::setupDrawMenu()
{
	m_drawMenu = new QMenu(tr("&Draw"), m_gridBirdEyeWindow);

	drawDisplaySettingAction = new QAction(tr("&Display Setting..."), m_drawMenu);
	connect(drawDisplaySettingAction, SIGNAL(triggered()), m_gridBirdEyeWindow, SLOT(displaySetting()));
	m_drawMenu->addAction(drawDisplaySettingAction);

	m_drawMenu->addSeparator();

	backgroundColorAction = new QAction(tr("&Background Color..."), m_drawMenu);
	backgroundColorAction->setIcon(QIcon(":/libs/guibase/images/iconColor.png"));
	connect(backgroundColorAction, SIGNAL(triggered()), m_gridBirdEyeWindow, SLOT(editBackgroundColor()));
	m_drawMenu->addAction(backgroundColorAction);

	editZScaleAction = new QAction(tr("&Z-direction Scale..."), m_drawMenu);
	connect(editZScaleAction, SIGNAL(triggered()), m_gridBirdEyeWindow, SLOT(editZScale()));
	m_drawMenu->addAction(editZScaleAction);
}
