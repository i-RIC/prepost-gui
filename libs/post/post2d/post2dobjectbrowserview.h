#ifndef POST2DOBJECTBROWSERVIEW_H
#define POST2DOBJECTBROWSERVIEW_H

#include <guibase/objectbrowserview.h>
#include <QPoint>

class QAction;

class Post2dObjectBrowserView : public ObjectBrowserView
{
	Q_OBJECT
public:
	Post2dObjectBrowserView(QWidget* parent);
};

#endif // POST2DOBJECTBROWSERVIEW_H
