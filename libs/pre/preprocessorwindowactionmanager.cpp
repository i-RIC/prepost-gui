#include "preprocessordatamodel.h"
#include "preprocessorwindow.h"
#include "preprocessorwindowactionmanager.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QAction>
#include <QIcon>
#include <QMenu>

PreProcessorWindowActionManager::PreProcessorWindowActionManager(PreProcessorWindow* parent) :
	QObject(parent),
	m_preWindow {parent}
{
	init();
}

void PreProcessorWindowActionManager::init()
{
	setupCalcCondMenu();
}

void PreProcessorWindowActionManager::setupCalcCondMenu()
{
	m_calcCondMenu = new QMenu(tr("&Calculation Condition"), m_preWindow);

	calcCondEditAction = new QAction(tr("&Setting..."), m_calcCondMenu);
	m_calcCondMenu->addAction(calcCondEditAction);

	m_calcCondMenu->addSeparator();

	calcCondImportAction = new QAction(tr("&Import..."), m_calcCondMenu);
	calcCondImportAction->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	m_calcCondMenu->addAction(calcCondImportAction);

	calcCondExportAction = new QAction(tr("&Export..."), m_calcCondMenu);
	calcCondExportAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	m_calcCondMenu->addAction(calcCondExportAction);
}

QMenu* PreProcessorWindowActionManager::calcCondMenu() const
{
	return m_calcCondMenu;
}

void PreProcessorWindowActionManager::connectWithDataModel()
{
	PreProcessorDataModel* model = dynamic_cast<PreProcessorDataModel*>(m_preWindow->m_dataModel);
	connect(calcCondEditAction, SIGNAL(triggered()), model, SLOT(showCalcConditionDialog()));

	connect(calcCondImportAction, SIGNAL(triggered()), model, SLOT(importCalcCondition()));
	connect(calcCondExportAction, SIGNAL(triggered()), model, SLOT(exportCalcCondition()));
}
