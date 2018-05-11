#include "../main/iricmainwindow.h"
#include "../solverdef/solverdefinitionabstractdialog.h"
#include "../solverdef/solverdefinitionlist.h"
#include "../solverdef/solverdefinitionlistdialog.h"
#include "../solverdef/solverdefinitiontranslationupdatewizard.h"
#include "installertool.h"
#include "iricmainwindowaboutdialog.h"
#include "iricmainwindowmiscdialogmanager.h"
#include "mousehelpdialog.h"

#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>

#include <QMessageBox>

iRICMainWindowMiscDialogManager::iRICMainWindowMiscDialogManager(iRICMainWindow* parent) :
	QObject(parent),
	m_mainWindow {parent},
	m_aboutDialog {new iRICMainWindowAboutDialog(m_mainWindow)}
{}

void iRICMainWindowMiscDialogManager::setupAboutDialog(const iRICMetaData& metaData)
{
	m_aboutDialog->init(metaData);
}

void iRICMainWindowMiscDialogManager::solverDefinitionTranslationWizard()
{
	SolverDefinitionTranslationUpdateWizard wizard(m_mainWindow);
	wizard.init(m_mainWindow->m_solverDefinitionList, m_mainWindow->m_gridCreatingConditionCreators);
	wizard.exec();
}

void iRICMainWindowMiscDialogManager::solverListDialog()
{
	SolverDefinitionListDialog* dialog = m_mainWindow->m_solverDefinitionList->dialog(m_mainWindow);
	dialog->exec();
}

void iRICMainWindowMiscDialogManager::aboutCurrentSolver()
{
	const SolverDefinitionAbstract* abst = &(m_mainWindow->m_projectData->solverDefinition()->abstract());
	SolverDefinitionAbstractDialog* dialog = new SolverDefinitionAbstractDialog(abst, m_mainWindow);
	dialog->exec();
}

void iRICMainWindowMiscDialogManager::mouseHelp()
{
	static MouseHelpDialog* dialog = nullptr;
	if (dialog == nullptr) {
		dialog = new MouseHelpDialog(m_mainWindow);
	}
	dialog->show();
}

void iRICMainWindowMiscDialogManager::about()
{
	m_aboutDialog->exec();
}

void iRICMainWindowMiscDialogManager::showMaintainanceDialog()
{
	InstallerTool::openMaintainanceDialog(m_mainWindow);
}
