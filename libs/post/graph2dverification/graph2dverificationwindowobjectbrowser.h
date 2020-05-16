#ifndef GRAPH2DVERIFICATIONWINDOWOBJECTBROWSER_H
#define GRAPH2DVERIFICATIONWINDOWOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Graph2dVerificationWindowObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Graph2dVerificationWindowObjectBrowser : public ObjectBrowser
{
	Q_OBJECT

public:
	/// Constructor
	Graph2dVerificationWindowObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~Graph2dVerificationWindowObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	Graph2dVerificationWindowObjectBrowserView* view() const {return m_view;}

public slots:
	void expandAll();

private:
	void init();
	/// The widget to show in the dockarea
	Graph2dVerificationWindowObjectBrowserView* m_view;
};

#endif // GRAPH2DVERIFICATIONWINDOWOBJECTBROWSER_H
