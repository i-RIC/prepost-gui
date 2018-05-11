#ifndef IRICMAINWINDOWMISCDIALOGMANAGER_H
#define IRICMAINWINDOWMISCDIALOGMANAGER_H

#include <QObject>

class iRICMetaData;
class iRICMainWindow;
class iRICMainWindowAboutDialog;

/// This class manages misc dialogs used from iRICMainWindow
class iRICMainWindowMiscDialogManager : public QObject
{
	Q_OBJECT

public:
	/// Constructor;
	iRICMainWindowMiscDialogManager(iRICMainWindow* parent);
	void setupAboutDialog(const iRICMetaData& metaData);

public slots:
	/// Show solver definition file translation update wizard
	void solverDefinitionTranslationWizard();
	/// Show solver list dialog
	void solverListDialog();
	/// Show solver information dialog about current solver.
	void aboutCurrentSolver();

	/// Show maintainance dialog
	void showMaintainanceDialog();
	/// Show mouse help dialog.
	void mouseHelp();
	/// Show about Dialog of iRIC
	void about();

private:
	iRICMainWindow* m_mainWindow;
	iRICMainWindowAboutDialog* m_aboutDialog;
};

#endif // IRICMAINWINDOWMISCDIALOGMANAGER_H
