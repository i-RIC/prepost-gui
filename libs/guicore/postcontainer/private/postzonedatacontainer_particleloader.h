#ifndef POSTZONEDATACONTAINER_PARTICLELOADER_H
#define POSTZONEDATACONTAINER_PARTICLELOADER_H

#include "../postzonedatacontainer.h"

#include <vtkSmartPointer.h>

class PostZoneDataContainer::ParticleLoader
{
public:
	static bool load(int fid, int bid, int zid, int step, vtkSmartPointer<vtkPolyData>* data, const QPointF& offset);

private:
	ParticleLoader();
};

#endif // POSTZONEDATACONTAINER_PARTICLELOADER_H
