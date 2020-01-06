#ifndef POST2DWINDOWPARTICLEGROUPROOTDATAITEM_H
#define POST2DWINDOWPARTICLEGROUPROOTDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowParticleGroupRootDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticleGroupRootDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticleGroupRootDataItem();

	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DWINDOWPARTICLEGROUPROOTDATAITEM_H
