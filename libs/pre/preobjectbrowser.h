#ifndef PREOBJECTBROWSER_H
#define PREOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class PreObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class PreObjectBrowser : public ObjectBrowser
{
	Q_OBJECT

public:
	/// Constructor
	PreObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~PreObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	PreObjectBrowserView* view() {return m_view;}

public slots:
	void expandAll();

private:
	void init();
	/// The widget to show in the dockarea
	PreObjectBrowserView* m_view;
};

#endif // PREOBJECTBROWSER_H
