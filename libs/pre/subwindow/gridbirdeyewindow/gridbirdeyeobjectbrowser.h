#ifndef GRIDBIRDEYEOBJECTBROWSER_H
#define GRIDBIRDEYEOBJECTBROWSER_H

#include <guibase/objectbrowser.h>

class GridBirdEyeObjectBrowserView;
class QStandardItemModel;

/// PreObjectBrowser class represents the ObjectBrowser shown in the
/// left region in PreProcessorWindow.
class GridBirdEyeObjectBrowser : public ObjectBrowser
{
	Q_OBJECT

public:
	GridBirdEyeObjectBrowser(QWidget* parent);
	~GridBirdEyeObjectBrowser();

	void setModel(QStandardItemModel* model);
	GridBirdEyeObjectBrowserView* view() const;

public slots:
	void expandAll();

private:
	void init();

	GridBirdEyeObjectBrowserView* m_view;
};

#endif // GRIDBIRDEYEOBJECTBROWSER_H
