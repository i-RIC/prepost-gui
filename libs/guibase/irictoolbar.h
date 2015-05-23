#ifndef IRICTOOLBAR_H
#define IRICTOOLBAR_H

#include "guibase_global.h"

#include <QToolBar>

/**
 * @brief Toolbar class used in iRIC
 *
 * The default icon size is changed to (16, 16), and signal visibilityChanged() is added.
 */
class GUIBASEDLL_EXPORT iRICToolBar : public QToolBar
{
	Q_OBJECT

public:
	iRICToolBar(const QString& t, QWidget* parent);
	iRICToolBar(QWidget* parent);

signals:
	void visibilityChanged(bool visible);

protected:
	void hideEvent(QHideEvent* e);
	void showEvent(QShowEvent* e);

private:
	void init();
};

#endif // IRICTOOLBAR_H
