#ifndef POST2DWINDOWROOTDATAITEM_H
#define POST2DWINDOWROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include "../post2dwindowdatamodel.h"
#include <QList>
#include <QMap>

class Post2dWindowBackgroundImagesDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowGridDataItem;
class Post2dWindowMeasuredDataTopDataItem;
class Post2dWindowDataModel;
class PostTitleDataItem;
class PostTimeDataItem;
class QStandardItemModel;
class Post2dWindow;
class Axis2dDataItem;
class DistanceMeasureGroupDataItem;
class AttributeBrowserTargetDataItem;

class Post2dWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post2dWindowRootDataItem(Post2dWindow* window, ProjectDataItem* parent);
	/// Destructor
	~Post2dWindowRootDataItem();

	/// Background image data item
	Post2dWindowBackgroundImagesDataItem* backgroundImagesDataItem() {
		return m_backgroundImagesDataItem;
	}
	/// Measured values data item;
	Post2dWindowMeasuredDataTopDataItem* measuredDataTopDataItem() {
		return m_measuredDataTopDataItem;
	}
	void setupStandardModel(QStandardItemModel* model);
	const QList<Post2dWindowGridTypeDataItem*>& gridTypeDataItems() {return m_gridTypeDataItems;}
	Post2dWindowGridTypeDataItem* gridTypeDataItem(const QString& name);
	Post2dWindowZoneDataItem* zoneDataItem(const QString& name);
	PostTitleDataItem* titleDataItem() {return m_titleDataItem;}
	PostTimeDataItem* timeDataItem() {return m_timeDataItem;}
	AttributeBrowserTargetDataItem* attributeBrowserTargetDataItem() const {return m_attributeBrowserTargetDataItem;}

private slots:
	void updateZoneList();
	void update();

signals:
	void standardModelSetuped();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	Post2dWindowBackgroundImagesDataItem* m_backgroundImagesDataItem;
	Post2dWindowMeasuredDataTopDataItem* m_measuredDataTopDataItem;
	QList<Post2dWindowGridTypeDataItem*> m_gridTypeDataItems;
	PostTitleDataItem* m_titleDataItem;
	PostTimeDataItem* m_timeDataItem;
	Axis2dDataItem* m_axesDataItem;
	DistanceMeasureGroupDataItem* m_distanceMeasureGroupDataItem;
	AttributeBrowserTargetDataItem* m_attributeBrowserTargetDataItem;

public:
	friend class Post2dWindowProjectDataItem;
	friend class Post2dWindowDataModel;
};

#endif // POST2DWINDOWROOTDATAITEM_H
