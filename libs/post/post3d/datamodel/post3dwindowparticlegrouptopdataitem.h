#ifndef POST3DWINDOWPARTICLEGROUPTOPDATAITEM_H
#define POST3DWINDOWPARTICLEGROUPTOPDATAITEM_H

#include "post3dwindowparticlesbasetopdataitem.h"

class Post3dWindowParticleGroupTopDataItem : public Post3dWindowParticlesBaseTopDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticleGroupTopDataItem(const std::string& name, Post3dWindowDataItem* parent);

	const std::string& name() const;

	v4Particles3d* particleData() const override;
	Post3dWindowZoneDataItem* zoneDataItem() const override;

private:
	std::string m_name;
};

#endif // POST3DWINDOWPARTICLEGROUPTOPDATAITEM_H
