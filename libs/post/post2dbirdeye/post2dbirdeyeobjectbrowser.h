#ifndef POST2DBIRDEYEOBJECTBROWSER_H
#define POST2DBIRDEYEOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Post2dBirdEyeObjectBrowserView;
class QStandardItemModel;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class Post2dBirdEyeObjectBrowser : public ObjectBrowser
{
	Q_OBJECT

public:
	Post2dBirdEyeObjectBrowser(QWidget* parent);
	~Post2dBirdEyeObjectBrowser();

	void setModel(QStandardItemModel* model);
	Post2dBirdEyeObjectBrowserView* view() const;

public slots:
	void expandAll();

private:
	void init();

	Post2dBirdEyeObjectBrowserView* m_view;
};

#endif // POST2DBIRDEYEOBJECTBROWSER_H
