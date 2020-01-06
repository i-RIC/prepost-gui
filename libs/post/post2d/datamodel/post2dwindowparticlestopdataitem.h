#ifndef POST2DWINDOWPARTICLESTOPDATAITEM_H
#define POST2DWINDOWPARTICLESTOPDATAITEM_H

#include "post2dwindowparticlesbasetopdataitem.h"

class Post2dWindowParticlesTopDataItem : public Post2dWindowParticlesBaseTopDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* parent);

	vtkPolyData* particleData() const override;
	Post2dWindowZoneDataItem* zoneDataItem() const override;
};

#endif // POST2DWINDOWPARTICLESTOPDATAITEM_H
