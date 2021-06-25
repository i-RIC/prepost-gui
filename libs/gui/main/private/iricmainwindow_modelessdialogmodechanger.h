#ifndef IRICMAINWINDOW_MODELESSDIALOGMODECHANGER_H
#define IRICMAINWINDOW_MODELESSDIALOGMODECHANGER_H

#include "../iricmainwindow.h"

class iRICMainWindow::ModelessDialogModeChanger
{
public:
	ModelessDialogModeChanger(iRICMainWindow* window);
	~ModelessDialogModeChanger();

private:
	iRICMainWindow* m_window;
};

#endif // IRICMAINWINDOW_MODELESSDIALOGMODECHANGER_H
