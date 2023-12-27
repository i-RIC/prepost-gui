#ifndef POST3DWINDOWROOTDATAITEM_H
#define POST3DWINDOWROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include "../post3dwindowdatamodel.h"
#include <QList>
#include <QMap>

class Axis3dDataItem;
class LogoDataItem;
class Post3dWindow;
class Post3dWindowGridTypeDataItem;
class Post3dWindowZoneDataItem;
class Post3dWindowDataModel;
class PostTitleDataItem;
class PostTimeDataItem;

class QStandardItemModel;

class Post3dWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT

public:
	Post3dWindowRootDataItem(Post3dWindow* window, ProjectDataItem* parent);
	~Post3dWindowRootDataItem();

	void setupStandardModel(QStandardItemModel* model);
	const QList<Post3dWindowGridTypeDataItem*>& gridTypeDataItems() const;
	Post3dWindowGridTypeDataItem* gridTypeDataItem(const std::string& name) const;
	Post3dWindowZoneDataItem* zoneDataItem(const std::string& name);

	PostTitleDataItem* titleDataItem() const;
	PostTimeDataItem* timeDataItem() const;

private slots:
	void updateZoneList();
	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QList<Post3dWindowGridTypeDataItem*> m_gridTypeDataItems;
	PostTitleDataItem* m_titleDataItem;
	PostTimeDataItem* m_timeDataItem;
	Axis3dDataItem* m_axesDataItem;
	LogoDataItem* m_logoDataItem;

public:
	friend class Post3dWindowProjectDataItem;
	friend class Post3dWindowDataModel;
};

#endif // POST3DWINDOWROOTDATAITEM_H
