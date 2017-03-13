#ifndef OBJECTBROWSERVIEW_H
#define OBJECTBROWSERVIEW_H

#include <QTreeView>

class ObjectBrowserView : public QTreeView
{
	Q_OBJECT

public:
	ObjectBrowserView(QWidget* w);
	~ObjectBrowserView();
};

#endif // OBJECTBROWSERVIEW_H
