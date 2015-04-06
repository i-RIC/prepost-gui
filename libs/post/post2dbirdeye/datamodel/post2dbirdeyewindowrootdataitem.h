#ifndef POST2DBIRDEYEWINDOWROOTDATAITEM_H
#define POST2DBIRDEYEWINDOWROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include "../post2dbirdeyewindowdatamodel.h"
#include <QList>
#include <QMap>

class Post2dBirdEyeWindowBackgroundImagesDataItem;
class Post2dBirdEyeWindowGridTypeDataItem;
class Post2dBirdEyeWindowGridDataItem;
class Post2dBirdEyeWindowDataModel;
class PostTitleDataItem;
class PostTimeDataItem;
class QStandardItemModel;
class Post2dBirdEyeWindow;
class Post2dBirdEyeWindowAxesDataItem;

class Post2dBirdEyeWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dBirdEyeWindowRootDataItem(Post2dBirdEyeWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Post2dBirdEyeWindowRootDataItem();

	/// Background image data item
	Post2dBirdEyeWindowBackgroundImagesDataItem* backgroundImagesDataItem(){
		return m_backgroundImagesDataItem;
	}
	void setupStandardModel(QStandardItemModel* model);
	const QList<Post2dBirdEyeWindowGridTypeDataItem*>& gridTypeDataItems(){return m_gridTypeDataItems;}
	Post2dBirdEyeWindowGridTypeDataItem* gridTypeDataItem(const QString& name);
	Post2dBirdEyeWindowZoneDataItem* zoneDataItem(const QString& name);
	PostTitleDataItem* titleDataItem(){return m_titleDataItem;}
	PostTimeDataItem* timeDataItem(){return m_timeDataItem;}
private slots:
	void updateZoneList();
	void update();
signals:
	void standardModelSetuped();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	Post2dBirdEyeWindowBackgroundImagesDataItem* m_backgroundImagesDataItem;
	QList<Post2dBirdEyeWindowGridTypeDataItem*> m_gridTypeDataItems;
	PostTitleDataItem* m_titleDataItem;
	PostTimeDataItem* m_timeDataItem;
	Post2dBirdEyeWindowAxesDataItem* m_axesDataItem;
public:
	friend class Post2dBirdEyeWindowProjectDataItem;
	friend class Post2dBirdEyeWindowDataModel;
};

#endif // POST2DBIRDEYEWINDOWROOTDATAITEM_H
