#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowactionmanager.h"
#include "graph2dverificationwindowdatamodel.h"

#include <QAction>
#include <QIcon>
#include <QMenu>

Graph2dVerificationWindowActionManager::Graph2dVerificationWindowActionManager(Graph2dVerificationWindow* parent) :
	QObject(parent),
	m_graphWindow {parent}
{
	init();
}

void Graph2dVerificationWindowActionManager::init()
{
	setupDrawMenu();
}

void Graph2dVerificationWindowActionManager::setupDrawMenu()
{
	m_drawMenu = new QMenu(tr("&Draw"), m_graphWindow);

	drawDatasourceSettingAction = new QAction(tr("&Data Source Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawDatasourceSettingAction);

	m_drawMenu->addSeparator();
	drawLegendAction = new QAction(tr("&Legend"), m_drawMenu);
	drawLegendAction->setCheckable(true);
	drawLegendAction->setChecked(true);
	m_drawMenu->addAction(drawLegendAction);
}

void Graph2dVerificationWindowActionManager::connectWithDataModel()
{
	connect(drawDatasourceSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(dataSourceSetting()));
	connect(drawLegendAction, SIGNAL(triggered(bool)), m_graphWindow->m_dataModel, SLOT(setLegendVisible(bool)));
}
