#include "irictoolbar.h"

iRICToolBar::iRICToolBar(const QString& t, QWidget* parent) :
	QToolBar(t, parent)
{}

iRICToolBar::iRICToolBar(QWidget* parent) :
	iRICToolBar("", parent)
{}

void iRICToolBar::hideEvent(QHideEvent* e)
{
	emit visibilityChanged(false);
	QToolBar::hideEvent(e);
}

void iRICToolBar::showEvent(QShowEvent* e)
{
	emit visibilityChanged(true);
	QToolBar::showEvent(e);
}
