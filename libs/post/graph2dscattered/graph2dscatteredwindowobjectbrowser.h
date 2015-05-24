#ifndef GRAPH2DSCATTEREDWINDOWOBJECTBROWSER_H
#define GRAPH2DSCATTEREDWINDOWOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Graph2dScatteredWindowObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Graph2dScatteredWindowObjectBrowser : public ObjectBrowser
{
	Q_OBJECT
public:
	/// Constructor
	Graph2dScatteredWindowObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~Graph2dScatteredWindowObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	Graph2dScatteredWindowObjectBrowserView* view() const {return m_view;}

public slots:
	void expandAll();

private:
	void init();
	/// The widget to show in the dockarea
	Graph2dScatteredWindowObjectBrowserView* m_view;
};

#endif // GRAPH2DSCATTEREDWINDOWOBJECTBROWSER_H
