#ifndef POST2DBIRDEYEOBJECTBROWSER_H
#define POST2DBIRDEYEOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Post2dBirdEyeObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Post2dBirdEyeObjectBrowser : public ObjectBrowser
{
	Q_OBJECT

public:
	/// Constructor
	Post2dBirdEyeObjectBrowser(QWidget* parent) : ObjectBrowser(parent) {init();}
	/// Destructor
	~Post2dBirdEyeObjectBrowser() {}
	void setModel(QStandardItemModel* model);
	Post2dBirdEyeObjectBrowserView* view() const {return m_view;}

public slots:
	void expandAll();

private:
	void init();
	/// The widget to show in the dockarea
	Post2dBirdEyeObjectBrowserView* m_view;
};

#endif // POST2DBIRDEYEOBJECTBROWSER_H
