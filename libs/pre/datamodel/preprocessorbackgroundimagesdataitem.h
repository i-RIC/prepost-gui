#ifndef PREPROCESSORBACKGROUNDIMAGESDATAITEM_H
#define PREPROCESSORBACKGROUNDIMAGESDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>

class PreProcessorBackgroundImagesDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	PreProcessorBackgroundImagesDataItem(GraphicsWindowDataItem* parent);
	void updateZDepthRangeItemCount() {m_zDepthRange.setItemCount(10);}
	void addCustomMenuItems(QMenu* menu) override;
	void visiblePropBounds(double bounds[6]);
	void handleStandardItemChange();
public slots:
	void addChildItem();
	void deleteChildItem(int i);
	void moveUpChildItem(int i);
	void moveDownChildItem(int i);
	void deleteAll();
	void updateChildCheckState(int idx, bool vis);
signals:
	void selectBackgroundImage(const QModelIndex& current);
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) {}

	QAction* m_addAction;
	QAction* m_deleteAllAction;
};

#endif // PREPROCESSORBACKGROUNDIMAGESDATAITEM_H
