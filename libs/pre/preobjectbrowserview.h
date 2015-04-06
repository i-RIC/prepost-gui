#ifndef PREOBJECTBROWSERVIEW_H
#define PREOBJECTBROWSERVIEW_H

#include <guibase/objectbrowserview.h>

#include <QTreeView>
#include <QPoint>

class QAction;

class PreObjectBrowserView : public ObjectBrowserView
{
	Q_OBJECT
public:
	PreObjectBrowserView(QWidget* parent);
};

#endif // PREOBJECTBROWSERVIEW_H
