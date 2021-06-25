#include "iricmainwindow_modelessdialogmodechanger.h"

#include <QCoreApplication>

iRICMainWindow::ModelessDialogModeChanger::ModelessDialogModeChanger(iRICMainWindow* window) :
	m_window {window}
{
	m_window->enterModelessDialogMode();
	qApp->processEvents();
}

iRICMainWindow::ModelessDialogModeChanger::~ModelessDialogModeChanger()
{
	m_window->exitModelessDialogMode();
	qApp->processEvents();
}
