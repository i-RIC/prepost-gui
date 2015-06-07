#include "graph2dscatteredwindow.h"
#include "graph2dscatteredwindowactionmanager.h"
#include "graph2dscatteredwindowdatamodel.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

Graph2dScatteredWindowActionManager::Graph2dScatteredWindowActionManager(Graph2dScatteredWindow* parent)
	: QObject(parent)
{
	m_graphWindow = parent;
	init();
}

void Graph2dScatteredWindowActionManager::init()
{
	setupDrawMenu();
}

void Graph2dScatteredWindowActionManager::setupDrawMenu()
{
	m_drawMenu = new QMenu(tr("&Draw"), m_graphWindow);

	drawDatasourceSettingAction = new QAction(tr("&Data Source Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawDatasourceSettingAction);

	m_drawMenu->addSeparator();

	drawAxisSettingAction = new QAction(tr("&Axis Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawAxisSettingAction);

	drawDrawSettingAction = new QAction(tr("D&raw Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawDrawSettingAction);

	m_drawMenu->addSeparator();

	drawLegendAction = new QAction(tr("&Legend"), m_drawMenu);
	drawLegendAction->setCheckable(true);
	drawLegendAction->setChecked(true);
	m_drawMenu->addAction(drawLegendAction);
}

void Graph2dScatteredWindowActionManager::connectWithDataModel()
{
	connect(drawDatasourceSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(dataSourceSetting()));
	connect(drawDrawSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(drawSetting()));
	connect(drawAxisSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(axisSetting()));
	connect(drawLegendAction, SIGNAL(triggered(bool)), m_graphWindow->m_dataModel, SLOT(setLegendVisible(bool)));
}
