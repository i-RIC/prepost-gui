#ifndef POST2DWINDOWPARTICLESTOPDATAITEM_H
#define POST2DWINDOWPARTICLESTOPDATAITEM_H

#include "post2dwindowparticlesbasetopdataitem.h"

class Post2dWindowParticlesTopDataItem : public Post2dWindowParticlesBaseTopDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* parent);

	v4Particles2d* particleData() const override;
	Post2dWindowCalculationResultDataItem* resultDataItem() const override;
};

#endif // POST2DWINDOWPARTICLESTOPDATAITEM_H
