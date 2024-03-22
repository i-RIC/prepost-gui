#ifndef V4POSTZONEDATACONTAINER_IMPL_H
#define V4POSTZONEDATACONTAINER_IMPL_H

#include "../v4postzonedatacontainer.h"

class v4PostZoneDataContainer::Impl
{
public:
	Impl();
	~Impl();

	void addCalculatedResultArrays();
	void clearParticleDataAndPolyData();

	std::string m_zoneName;
	SolverDefinitionGridType* m_gridType;

	v4InputGrid* m_inputGridData;
	v4SolutionGrid* m_gridData;
	v4SolutionGrid* m_particleData;
	std::map<std::string, v4SolutionGrid*> m_particleGroupMap;
	std::map<std::string, v4SolutionGrid*> m_particleGroupImageMap;
	std::map<std::string, v4SolutionGrid*> m_polyDataMap;

	std::vector<v4PostCalculatedResult*> m_calculatedResults;
};

#endif // V4POSTZONEDATACONTAINER_IMPL_H
