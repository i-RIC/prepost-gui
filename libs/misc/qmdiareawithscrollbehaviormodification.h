#ifndef QMDIAREAWITHSCROLLBEHAVIORMODIFICATION_H
#define QMDIAREAWITHSCROLLBEHAVIORMODIFICATION_H

#include "misc_global.h"

#include <QMdiArea>

class MISCDLL_EXPORT QMdiAreaWithScrollBehaviorModification : public QMdiArea
{
public:
	QMdiAreaWithScrollBehaviorModification(QWidget* parent);

	void wheelEvent(QWheelEvent *e);
};

#endif // QMDIAREAWITHSCROLLBEHAVIORMODIFICATION_H
