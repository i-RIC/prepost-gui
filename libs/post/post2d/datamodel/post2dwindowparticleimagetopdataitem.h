#ifndef POST2DWINDOWPARTICLEIMAGETOPDATAITEM_H
#define POST2DWINDOWPARTICLEIMAGETOPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowCalculationResultDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowParticleImageTopDataItem : public Post2dWindowDataItem
{
public:
	Post2dWindowParticleImageTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticleImageTopDataItem();

	void update();

	Post2dWindowCalculationResultDataItem* resultDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DWINDOWPARTICLEIMAGETOPDATAITEM_H
