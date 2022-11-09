#ifndef POST2DWINDOWBACKGROUNDIMAGESDATAITEM_H
#define POST2DWINDOWBACKGROUNDIMAGESDATAITEM_H

#include "../post2dwindowdataitem.h"

class QAction;

class Post2dWindowBackgroundImagesDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowBackgroundImagesDataItem(GraphicsWindowDataItem* parent);
	~Post2dWindowBackgroundImagesDataItem();
	void updateZDepthRangeItemCount() override {m_zDepthRange.setItemCount(10);}
	void addCustomMenuItems(QMenu* menu) override;
	void visiblePropBounds(double bounds[6]);

public slots:
	void addChildItem();
	void deleteChildItem(int i);
	void moveUpChildItem(int i);
	void moveDownChildItem(int i);
	void setupChildItem();

private slots:
	void deleteSelected();
	void deleteAll();

signals:
	void selectBackgroundImage(const QModelIndex& current);
	void requestRemoveRenderer(vtkRenderer* ren);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QAction* m_addAction;
	QAction* m_deleteSelectedAction;
	QAction* m_deleteAllAction;
};

#endif // POST2DWINDOWBACKGROUNDIMAGESDATAITEM_H
