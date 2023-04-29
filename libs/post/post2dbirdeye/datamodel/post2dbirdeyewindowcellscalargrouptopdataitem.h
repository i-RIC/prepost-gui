#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

class Post2dBirdEyeWindowCellScalarGroupDataItem;
class Post2dBirdEyeWindowZoneDataItem;

class Post2dBirdEyeWindowCellScalarGroupTopDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowCellScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowCellScalarGroupTopDataItem();

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

	friend class Post2dBirdEyeWindowCellScalarGroupDataItem;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPTOPDATAITEM_H
