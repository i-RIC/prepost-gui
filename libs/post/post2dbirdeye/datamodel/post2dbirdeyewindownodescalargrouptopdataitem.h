#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <QMap>

class Post2dBirdEyeWindowNodeScalarGroupDataItem;
class Post2dBirdEyeWindowZoneDataItem;

class Post2dBirdEyeWindowNodeScalarGroupTopDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
public:
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupTopDataItem();

	void update();
	Post2dBirdEyeWindowZoneDataItem* zoneDataItem() const;

private:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* addDialog) override;
	void innerUpdateZScale(double scale) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	double m_zScale;

	friend class Post2dBirdEyeWindowNodeScalarGroupDataItem;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
