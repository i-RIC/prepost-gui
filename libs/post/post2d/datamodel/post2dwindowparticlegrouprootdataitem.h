#ifndef POST2DWINDOWPARTICLEGROUPROOTDATAITEM_H
#define POST2DWINDOWPARTICLEGROUPROOTDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowCalculationResultDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowParticleGroupRootDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticleGroupRootDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticleGroupRootDataItem();

	void update();

private:
	Post2dWindowCalculationResultDataItem* resultDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DWINDOWPARTICLEGROUPROOTDATAITEM_H
