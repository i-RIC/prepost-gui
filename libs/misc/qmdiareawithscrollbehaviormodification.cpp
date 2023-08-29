#include "qmdiareawithscrollbehaviormodification.h"

QMdiAreaWithScrollBehaviorModification::QMdiAreaWithScrollBehaviorModification(QWidget* parent) :
	QMdiArea {parent}
{}

void QMdiAreaWithScrollBehaviorModification::wheelEvent(QWheelEvent *e)
{
	if ((e->modifiers() & Qt::ControlModifier) != 0) {return;}
	QMdiArea::wheelEvent(e);
}
