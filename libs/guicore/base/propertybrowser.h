#ifndef PROPERTYBROWSER_H
#define PROPERTYBROWSER_H

#include "../guicore_global.h"

#include <QDockWidget>

class PropertyBrowserView;

class GUICOREDLL_EXPORT PropertyBrowser : public QDockWidget
{
	Q_OBJECT
public:
	PropertyBrowser(QWidget* parent = nullptr);
	virtual ~PropertyBrowser();

	PropertyBrowserView* view() const;

private:
	void hideEvent(QHideEvent*) override;

	PropertyBrowserView* m_view;
};

#endif // PROPERTYBROWSER_H
