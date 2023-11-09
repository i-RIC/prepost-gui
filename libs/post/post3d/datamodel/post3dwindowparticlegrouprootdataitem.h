#ifndef POST3DWINDOWPARTICLEGROUPROOTDATAITEM_H
#define POST3DWINDOWPARTICLEGROUPROOTDATAITEM_H

#include "../post3dwindowdataitem.h"

Post3dWindowZoneDataItem;

class Post3dWindowParticleGroupRootDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticleGroupRootDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowParticleGroupRootDataItem();

	void update();
	Post3dWindowZoneDataItem* zoneDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST3DWINDOWPARTICLEGROUPROOTDATAITEM_H
