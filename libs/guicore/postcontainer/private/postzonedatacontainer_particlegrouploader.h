#ifndef POSTZONEDATACONTAINER_PARTICLEGROUPLOADER_H
#define POSTZONEDATACONTAINER_PARTICLEGROUPLOADER_H

#include "../postzonedatacontainer.h"

#include <map>
#include <string>

#include <vtkSmartPointer.h>

namespace iRICLib {
	class H5CgnsParticleGroupSolution;
} // namespace iRICLib

class QPointF;
class vtkPolyData;

class PostZoneDataContainer::ParticleGroupLoader
{
public:
	static bool load(std::map<std::string, vtkSmartPointer<vtkPolyData> >* particleGroupMap, iRICLib::H5CgnsParticleGroupSolution* sol, const QPointF& offset);

private:
	static int loadParticleGroup(const std::string& name, vtkPolyData* polyData, iRICLib::H5CgnsParticleGroupSolution* sol, const QPointF& offset);

	ParticleGroupLoader();
};

#endif // POSTZONEDATACONTAINER_PARTICLEGROUPLOADER_H
