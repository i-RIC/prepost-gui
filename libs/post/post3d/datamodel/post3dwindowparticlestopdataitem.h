#ifndef POST3DWINDOWPARTICLESTOPDATAITEM_H
#define POST3DWINDOWPARTICLESTOPDATAITEM_H

#include "post3dwindowparticlesbasetopdataitem.h"

class Post3dWindowParticlesTopDataItem : public Post3dWindowParticlesBaseTopDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* parent);

	v4Particles3d* particleData() const override;
	Post3dWindowZoneDataItem* zoneDataItem() const override;
};

#endif // POST3DWINDOWPARTICLESTOPDATAITEM_H
