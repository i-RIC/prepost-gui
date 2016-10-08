#ifndef POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

class Post3dWindowContourGroupTopDataItem : public Post3dWindowDataItem
{
public:
	Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p);
	~Post3dWindowContourGroupTopDataItem();

	void innerUpdateZScale(double scale) override;
	void update();

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

private:
	double m_zScale;
};

#endif // POST3DWINDOWCONTOURGROUPTOPDATAITEM_H
