#ifndef POST2DOBJECTBROWSER_H
#define POST2DOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class Post2dObjectBrowserView;
class QStandardItemModel;
class QModelIndex;

class Post2dObjectBrowser : public ObjectBrowser
{
	Q_OBJECT

public:
	Post2dObjectBrowser(QWidget* parent);
	~Post2dObjectBrowser();

	void setModel(QStandardItemModel* model);
	Post2dObjectBrowserView* view() const;

public slots:
	void expandAll();

private:
	Post2dObjectBrowserView* m_view;
};

#endif // POST2DOBJECTBROWSER_H
