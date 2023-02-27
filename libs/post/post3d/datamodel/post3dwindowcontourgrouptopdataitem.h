#ifndef POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <QMap>

class Post3dWindowContourGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p);

	double zScale() const;
	void innerUpdateZScale(double scale) override;
	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	void addCustomMenuItems(QMenu* menu) override;

	QDialog* addDialog(QWidget* p) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	double m_zScale;

public:
	friend class Post3dWindowContourGroupDataItem;
};

#endif // POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
