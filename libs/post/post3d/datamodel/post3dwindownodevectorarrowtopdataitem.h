#ifndef POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

class Post3dWindowNodeVectorArrowTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* parent);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST3DWINDOWNODEVECTORARROWTOPDATAITEM_H
