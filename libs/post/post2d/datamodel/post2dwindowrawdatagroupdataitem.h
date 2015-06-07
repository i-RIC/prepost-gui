#ifndef POST2DWINDOWRAWDATAGROUPDATAITEM_H
#define POST2DWINDOWRAWDATAGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <QString>
#include <QMap>

class SolverDefinitionGridAttribute;
class Post2dWindowRawDataDataItem;

class Post2dWindowRawDataGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowRawDataGroupDataItem(SolverDefinitionGridAttribute* cond, Post2dWindowDataItem* parent);
	void updateChildren();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
private:
	QMap<QString, Post2dWindowRawDataDataItem*> m_itemNameMap;
	SolverDefinitionGridAttribute* m_condition;
};

#endif // POST2DWINDOWRAWDATAGROUPDATAITEM_H
