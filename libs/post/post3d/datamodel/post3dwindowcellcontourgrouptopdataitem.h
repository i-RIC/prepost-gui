#ifndef POST3DWINDOWCELLCONTOURGROUPTOPDATAITEM_H
#define POST3DWINDOWCELLCONTOURGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

class Post3dWindowZoneDataItem;

class Post3dWindowCellContourGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowCellContourGroupTopDataItem(Post3dWindowDataItem* p);

	double zScale() const;
	void innerUpdateZScale(double scale) override;
	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	void addCustomMenuItems(QMenu* menu) override;

	QDialog* addDialog(QWidget* p) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	Post3dWindowZoneDataItem* zoneDataItem() const;

	double m_zScale;

public:
	friend class Post3dWindowCellContourGroupDataItem;
};

#endif // POST3DWINDOWCELLCONTOURGROUPTOPDATAITEM_H
