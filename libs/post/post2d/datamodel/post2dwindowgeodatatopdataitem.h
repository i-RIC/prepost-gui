#ifndef POST2DWINDOWGEODATATOPDATAITEM_H
#define POST2DWINDOWGEODATATOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <QString>
#include <QMap>

#include <string>

class PreProcessorGeoDataTopDataItemInterface;
class Post2dWindowGeoDataGroupDataItem;

class Post2dWindowGeoDataTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGeoDataTopDataItem(PreProcessorGeoDataTopDataItemInterface* ditem, Post2dWindowDataItem* parent);
	PreProcessorGeoDataTopDataItemInterface* preGeoDataTopDataItem() const {return m_preGeoDataTopDataItem;}

private slots:
	void updateChildren();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QMap<std::string, Post2dWindowGeoDataGroupDataItem*> m_itemNameMap;
	PreProcessorGeoDataTopDataItemInterface* m_preGeoDataTopDataItem;
};

#endif // POST2DWINDOWGEODATATOPDATAITEM_H
