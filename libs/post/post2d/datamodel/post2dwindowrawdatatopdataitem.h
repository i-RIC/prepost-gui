#ifndef POST2DWINDOWRAWDATATOPDATAITEM_H
#define POST2DWINDOWRAWDATATOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <QString>
#include <QMap>

class PreProcessorRawDataTopDataItemInterface;
class Post2dWindowRawDataGroupDataItem;

class Post2dWindowRawDataTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowRawDataTopDataItem(PreProcessorRawDataTopDataItemInterface* ditem, Post2dWindowDataItem* parent);
	PreProcessorRawDataTopDataItemInterface* preRawDataTopDataItem() const {return m_preRawDataTopDataItem;}

private slots:
	void updateChildren();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QMap<QString, Post2dWindowRawDataGroupDataItem*> m_itemNameMap;
	PreProcessorRawDataTopDataItemInterface* m_preRawDataTopDataItem;

};

#endif // POST2DWINDOWRAWDATATOPDATAITEM_H
