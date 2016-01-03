#include "irictoolbar.h"

iRICToolBar::iRICToolBar(const QString& t, QWidget* parent) :
	QToolBar(t, parent)
{
	setIconSize(QSize(16, 16));
}

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
