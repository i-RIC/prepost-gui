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
	void updateZDepthRangeItemCount() {m_zDepthRange.setItemCount(10);}
	void addCustomMenuItems(QMenu* menu);
	void visiblePropBounds(double bounds[6]);
	void handleStandardItemChange();

public slots:
	void addChildItem();
	void deleteChildItem(int i);
	void moveUpChildItem(int i);
	void moveDownChildItem(int i);
	void setupChildItem();
	void deleteAll();
	void updateChildCheckState(int idx, bool vis);

signals:
	void selectBackgroundImage(const QModelIndex& current);
	void requestRemoveRenderer(vtkRenderer* ren);

protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) {}

	QAction* m_addAction;
	QAction* m_deleteAllAction;
};

#endif // POST2DWINDOWBACKGROUNDIMAGESDATAITEM_H
