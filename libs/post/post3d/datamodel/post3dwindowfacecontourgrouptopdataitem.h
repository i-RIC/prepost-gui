#ifndef POST3DWINDOWFACECONTOURGROUPTOPDATAITEM_H
#define POST3DWINDOWFACECONTOURGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guicore/postcontainer/v4solutiongrid.h>

class Post3dWindowZoneDataItem;

class Post3dWindowFaceContourGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowFaceContourGroupTopDataItem(const QString& caption, v4SolutionGrid::Position pos, Post3dWindowDataItem* p);

	double zScale() const;
	void innerUpdateZScale(double scale) override;
	void update();

	v4SolutionGrid::Position position() const;

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	void addCustomMenuItems(QMenu* menu) override;

	QDialog* addDialog(QWidget* p) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	Post3dWindowZoneDataItem* zoneDataItem() const;

	double m_zScale;
	v4SolutionGrid::Position m_position;

public:
	friend class Post3dWindowFaceContourGroupDataItem;
};

#endif // POST3DWINDOWFACECONTOURGROUPTOPDATAITEM_H
