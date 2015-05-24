#ifndef ADDITIONALMENUWINDOWINTERFACE_H
#define ADDITIONALMENUWINDOWINTERFACE_H

#include <QList>
class QMenu;
class QToolBar;

class AdditionalMenuWindowInterface
{
public:
	virtual QList<QMenu*> getAdditionalMenus() const = 0;
	virtual QToolBar* getAdditionalToolBar() const {return 0;}
};

#endif // ADDITIONALMENUWINDOWINTERFACE_H
