#ifndef POST3DOBJECTBROWSER_H
#define POST3DOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Post3dObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Post3dObjectBrowser : public ObjectBrowser
{
	Q_OBJECT
public:
	/// Constructor
	Post3dObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~Post3dObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	Post3dObjectBrowserView* view() const {return m_view;}

public slots:
	void expandAll();

private:
	void init();
	/// The widget to show in the dockarea
	Post3dObjectBrowserView* m_view;
};

#endif // POST3DOBJECTBROWSER_H
