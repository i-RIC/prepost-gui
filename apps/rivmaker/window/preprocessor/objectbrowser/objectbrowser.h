#ifndef OBJECTBROWSER_H
#define OBJECTBROWSER_H

#include <QDockWidget>

class ObjectBrowser : public QDockWidget
{
public:
	ObjectBrowser(QWidget* parent = nullptr);
	~ObjectBrowser();
};

#endif // OBJECTBROWSER_H
