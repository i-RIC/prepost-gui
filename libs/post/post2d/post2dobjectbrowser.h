#ifndef POST2DOBJECTBROWSER_H
#define POST2DOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Post2dObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Post2dObjectBrowser : public ObjectBrowser
{
	Q_OBJECT
public:
	/// Constructor
	Post2dObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~Post2dObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	Post2dObjectBrowserView* view() const {return m_view;}

public slots:
	void expandAll();

private:
	void init();
	/// The widget to show in the dockarea
	Post2dObjectBrowserView* m_view;
};

#endif // POST2DOBJECTBROWSER_H
