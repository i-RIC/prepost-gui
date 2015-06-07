#include "irictoolbar.h"

iRICToolBar::iRICToolBar(const QString& t, QWidget* parent)
	: QToolBar(t, parent)
{
	init();
}

iRICToolBar::iRICToolBar(QWidget* parent)
	: QToolBar(parent)
{
	init();
}

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

void iRICToolBar::init()
{
	setIconSize(QSize(16, 16));
}
