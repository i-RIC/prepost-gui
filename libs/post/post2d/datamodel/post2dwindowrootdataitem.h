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
class TmsImageGroupDataItem;

class Post2dWindowRootDataItem : public GraphicsWindowRootDataItem
{
	Q_OBJECT

public:
	Post2dWindowRootDataItem(Post2dWindow* window, ProjectDataItem* parent);
	~Post2dWindowRootDataItem();

	/// Background image data item
	Post2dWindowBackgroundImagesDataItem* backgroundImagesDataItem() const;
	TmsImageGroupDataItem* tmsGroupDataItem() const;
	/// Measured values data item;
	Post2dWindowMeasuredDataTopDataItem* measuredDataTopDataItem() const;
	void setupStandardModel(QStandardItemModel* model);
	const QList<Post2dWindowGridTypeDataItem*>& gridTypeDataItems() const;
	Post2dWindowGridTypeDataItem* gridTypeDataItem(const std::string& name);
	Post2dWindowZoneDataItem* zoneDataItem(const std::string& name);
	PostTitleDataItem* titleDataItem() const;
	PostTimeDataItem* timeDataItem() const;
	AttributeBrowserTargetDataItem* attributeBrowserTargetDataItem() const;

private slots:
	void updateZoneList();
	void update();

signals:
	void standardModelSetuped();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowBackgroundImagesDataItem* m_backgroundImagesDataItem;
	TmsImageGroupDataItem* m_tmsGroupDataItem;
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
