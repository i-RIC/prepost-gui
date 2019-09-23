#include "graph2dverificationwindow.h"
#include "graph2dverificationwindowactionmanager.h"
#include "graph2dverificationwindowdatamodel.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

Graph2dVerificationWindowActionManager::Graph2dVerificationWindowActionManager(Graph2dVerificationWindow* parent)
	: QObject(parent)
{
	m_graphWindow = parent;
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

	drawAxisSettingAction = new QAction(tr("&Axis Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawAxisSettingAction);

	drawDrawSettingAction = new QAction(tr("D&raw Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawDrawSettingAction);

	m_drawMenu->addSeparator();

	drawMarkerSettingAction = new QAction(tr("&Marker Setting..."), m_drawMenu);
	m_drawMenu->addAction(drawMarkerSettingAction);

	drawKPMarkerSettingAction = new QAction(tr("Add &KP Markers"), m_drawMenu);
	m_drawMenu->addAction(drawKPMarkerSettingAction);

	m_drawMenu->addSeparator();

	drawCopyAction = new QAction(tr("&Copy"), m_drawMenu);
	m_drawMenu->addAction(drawCopyAction);

	m_drawMenu->addSeparator();

	drawLegendAction = new QAction(tr("&Legend"), m_drawMenu);
	drawLegendAction->setCheckable(true);
	drawLegendAction->setChecked(true);
	m_drawMenu->addAction(drawLegendAction);
}

void Graph2dVerificationWindowActionManager::connectWithDataModel()
{
	connect(drawDatasourceSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(dataSourceSetting()));
	connect(drawCopyAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(copyCalculationResult()));
	connect(drawDrawSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(drawSetting()));
	connect(drawMarkerSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(markerSettiing()));
	connect(drawAxisSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(axisSetting()));
	connect(drawKPMarkerSettingAction, SIGNAL(triggered()), m_graphWindow->m_dataModel, SLOT(addKPMarkers()));
	connect(drawLegendAction, SIGNAL(triggered(bool)), m_graphWindow->m_dataModel, SLOT(setLegendVisible(bool)));
}
