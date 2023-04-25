#ifndef ADDITIONALMENUWINDOWI_H
#define ADDITIONALMENUWINDOWI_H

#include <QList>

#include <memory>

class QMenu;
class QToolBar;

class AdditionalMenuWindowI
{
public:
	virtual ~AdditionalMenuWindowI() {}

	virtual QList<QMenu*> getAdditionalMenus() const = 0;
	virtual const std::shared_ptr<QToolBar>& getAdditionalToolBar() const = 0;
};

#endif // ADDITIONALMENUWINDOWI_H
