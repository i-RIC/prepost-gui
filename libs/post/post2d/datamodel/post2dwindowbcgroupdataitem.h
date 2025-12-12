#ifndef POST2DWINDOWBCGROUPDATAITEM_H
#define POST2DWINDOWBCGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowBCGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowBCGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowBCGroupDataItem();
	void setupChildren();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override {};
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override {};
};

#endif // POST2DWINDOWBCGROUPDATAITEM_H
