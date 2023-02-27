#ifndef POSTZONEDATACONTAINER_PARTICLELOADER_H
#define POSTZONEDATACONTAINER_PARTICLELOADER_H

#include "../postzonedatacontainer.h"

#include <vtkSmartPointer.h>

namespace iRICLib {
	class H5CgnsParticleSolution;
} // namespace iRICLib

class PostZoneDataContainer::ParticleLoader
{
public:
	static bool load(vtkPointSetAndValueRangeSetT<vtkPolyData>** data, iRICLib::H5CgnsParticleSolution* sol, const QPointF& offset);

private:
	ParticleLoader();
};

#endif // POSTZONEDATACONTAINER_PARTICLELOADER_H
