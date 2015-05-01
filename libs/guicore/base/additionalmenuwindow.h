#ifndef ADDITIONALMENUWINDOW_H
#define ADDITIONALMENUWINDOW_H

#include <QList>
class QMenu;
class QToolBar;

class AdditionalMenuWindow
{
public:
	virtual QList<QMenu*> getAdditionalMenus() = 0;
	virtual QToolBar* getAdditionalToolBar() {return 0;}
};

#endif // ADDITIONALMENUWINDOW_H
