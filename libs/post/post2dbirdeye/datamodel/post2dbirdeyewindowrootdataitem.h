#ifndef POST2DBIRDEYEWINDOWROOTDATAITEM_H
#define POST2DBIRDEYEWINDOWROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include "../post2dbirdeyewindowdatamodel.h"
#include <QList>

class Axis3dDataItem;
class Post2dBirdEyeWindow;
class Post2dBirdEyeWindowGridTypeDataItem;
class Post2dBirdEyeWindowDataModel;
class PostTimeDataItem;
class PostTitleDataItem;

class QStandardItemModel;

class Post2dBirdEyeWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowRootDataItem(Post2dBirdEyeWindow* window, ProjectDataItem* parent);
	~Post2dBirdEyeWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	const QList<Post2dBirdEyeWindowGridTypeDataItem*>& gridTypeDataItems() const;
	Post2dBirdEyeWindowGridTypeDataItem* gridTypeDataItem(const std::string& name) const;
	Post2dBirdEyeWindowZoneDataItem* zoneDataItem(const std::string& name) const;

	PostTitleDataItem* titleDataItem() const;
	PostTimeDataItem* timeDataItem() const;

private slots:
	void updateZoneList();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QList<Post2dBirdEyeWindowGridTypeDataItem*> m_gridTypeDataItems;
	PostTitleDataItem* m_titleDataItem;
	PostTimeDataItem* m_timeDataItem;
	Axis3dDataItem* m_axesDataItem;

public:
	friend class Post2dBirdEyeWindowProjectDataItem;
	friend class Post2dBirdEyeWindowDataModel;
};

#endif // POST2DBIRDEYEWINDOWROOTDATAITEM_H
