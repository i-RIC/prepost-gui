#ifndef POST2DWINDOWPARTICLEGROUPTOPDATAITEM_H
#define POST2DWINDOWPARTICLEGROUPTOPDATAITEM_H

#include "post2dwindowparticlesbasetopdataitem.h"

class Post2dWindowParticleGroupTopDataItem : public Post2dWindowParticlesBaseTopDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticleGroupTopDataItem(const std::string& name, Post2dWindowDataItem* parent);

	const std::string& name() const;

	vtkPolyData* particleData() const override;
	Post2dWindowZoneDataItem* zoneDataItem() const override;

private:
	std::string m_name;
};

#endif // POST2DWINDOWPARTICLEGROUPTOPDATAITEM_H
