#ifndef POST2DWINDOWGEODATAGROUPDATAITEM_H
#define POST2DWINDOWGEODATAGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QMap>

class SolverDefinitionGridAttribute;
class Post2dWindowGeoDataDataItem;

class Post2dWindowGeoDataGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent);
	void updateChildren();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	QMap<QString, Post2dWindowGeoDataDataItem*> m_itemNameMap;
	SolverDefinitionGridAttribute* m_condition;
};

#endif // POST2DWINDOWGEODATAGROUPDATAITEM_H
