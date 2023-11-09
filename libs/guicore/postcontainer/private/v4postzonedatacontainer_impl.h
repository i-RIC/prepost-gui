#ifndef V4POSTZONEDATACONTAINER_IMPL_H
#define V4POSTZONEDATACONTAINER_IMPL_H

#include "../v4postzonedatacontainer.h"

class v4PostZoneDataContainer::Impl
{
public:
	Impl();
	~Impl();

	void clearParticleDataAndPolyData();

	std::string m_zoneName;
	SolverDefinitionGridType* m_gridType;

	v4InputGrid* m_inputGridData;
	v4SolutionGrid* m_gridData;
	v4SolutionGrid* m_particleData;
	std::map<std::string, v4SolutionGrid*> m_particleGroupMap;
	std::map<std::string, v4SolutionGrid*> m_polyDataMap;
};

#endif // V4POSTZONEDATACONTAINER_IMPL_H
