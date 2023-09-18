#include "../../pre/grid/v4inputgrid.h"
#include "../v4solutiongrid.h"
#include "v4postzonedatacontainer_impl.h"

v4PostZoneDataContainer::Impl::Impl() :
	m_inputGridData {nullptr},
	m_gridData {nullptr},
	m_particleData {nullptr}
{}

v4PostZoneDataContainer::Impl::~Impl()
{
	delete m_inputGridData;
	delete m_gridData;

	clearParticleDataAndPolyData();
}

void v4PostZoneDataContainer::Impl::clearParticleDataAndPolyData()
{
	delete m_particleData;
	m_particleData = nullptr;

	for (const auto& pair : m_particleGroupMap) {
		delete pair.second;
	}
	m_particleGroupMap.clear();

	for (const auto& pair : m_polyDataMap) {
		delete pair.second;
	}
	m_polyDataMap.clear();
}
