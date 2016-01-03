#ifndef ADDITIONALMENUWINDOWI_H
#define ADDITIONALMENUWINDOWI_H

#include <QList>
class QMenu;
class QToolBar;

class AdditionalMenuWindowI
{
public:
	virtual ~AdditionalMenuWindowI() {}

	virtual QList<QMenu*> getAdditionalMenus() const = 0;
	virtual QToolBar* getAdditionalToolBar() const = 0;
};

#endif // ADDITIONALMENUWINDOWI_H
