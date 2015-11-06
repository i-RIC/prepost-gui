#ifndef OBJECTBROWSER_H
#define OBJECTBROWSER_H

#include "guibase_global.h"

#include <QDockWidget>

/// Abstract class for object browser in sub-windows of iRIC
class GUIBASEDLL_EXPORT ObjectBrowser : public QDockWidget
{
	Q_OBJECT

public:
	ObjectBrowser(QWidget* parent = nullptr);
	virtual ~ObjectBrowser();
};

#endif // OBJECTBROWSER_H
