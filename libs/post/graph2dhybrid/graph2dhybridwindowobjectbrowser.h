#ifndef GRAPH2DHYBRIDWINDOWOBJECTBROWSER_H
#define GRAPH2DHYBRIDWINDOWOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Graph2dHybridWindowObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Graph2dHybridWindowObjectBrowser : public ObjectBrowser
{
	Q_OBJECT
public:
	/// Constructor
	Graph2dHybridWindowObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~Graph2dHybridWindowObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	Graph2dHybridWindowObjectBrowserView* view() {return m_view;}
public slots:
	void expandAll();
private:
	void init();
	/// The widget to show in the dockarea
	Graph2dHybridWindowObjectBrowserView* m_view;
};

#endif // GRAPH2DHYBRIDWINDOWOBJECTBROWSER_H
