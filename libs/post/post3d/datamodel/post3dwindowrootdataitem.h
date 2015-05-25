#ifndef POST3DWINDOWROOTDATAITEM_H
#define POST3DWINDOWROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include "../post3dwindowdatamodel.h"
#include <QList>
#include <QMap>

//class Post3dWindowBackgroundImagesDataItem;
class Post3dWindowGridTypeDataItem;
class Post3dWindowZoneDataItem;
class Post3dWindowDataModel;
class QStandardItemModel;
class PostTitleDataItem;
class PostTimeDataItem;
class Axis3dDataItem;
class Post3dWindow;

class Post3dWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowRootDataItem(Post3dWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Post3dWindowRootDataItem();

	/// Background image data item
//	Post3dWindowBackgroundImagesDataItem* backgroundImagesDataItem(){
//		return m_backgroundImagesDataItem;
//	}
	void setupStandardModel(QStandardItemModel* model);
	const QList<Post3dWindowGridTypeDataItem*>& gridTypeDataItems() const {return m_gridTypeDataItems;}
	Post3dWindowGridTypeDataItem* gridTypeDataItem(const QString& name) const;
	Post3dWindowZoneDataItem* zoneDataItem(const QString& name);
	PostTitleDataItem* titleDataItem() const {return m_titleDataItem;}
	PostTimeDataItem* timeDataItem() const {return m_timeDataItem;}

private slots:
	void updateZoneList();
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
private:
//	Post3dWindowBackgroundImagesDataItem* m_backgroundImagesDataItem;
	QList<Post3dWindowGridTypeDataItem*> m_gridTypeDataItems;
	PostTitleDataItem* m_titleDataItem;
	PostTimeDataItem* m_timeDataItem;
	Axis3dDataItem* m_axesDataItem;
public:
	friend class Post3dWindowProjectDataItem;
	friend class Post3dWindowDataModel;
};

#endif // POST3DWINDOWROOTDATAITEM_H
