#ifndef PROPERTYBROWSER_H
#define PROPERTYBROWSER_H

#include "../guicore_global.h"

#include <QDockWidget>

class PropertyBrowserView;

class GUICOREDLL_EXPORT PropertyBrowser : public QDockWidget
{
		Q_OBJECT
public:
	PropertyBrowser(QWidget *parent = 0);
	virtual ~PropertyBrowser();
	PropertyBrowserView* view() const {return m_view;}

protected:
	void hideEvent(QHideEvent *);

private:
	PropertyBrowserView* m_view;
};

#endif // PROPERTYBROWSER_H
