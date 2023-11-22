#include "../../pre/grid/v4inputgrid.h"
#include "../v4postcalculatedresult.h"
#include "../v4solutiongrid.h"
#include "v4postzonedatacontainer_impl.h"

namespace {

vtkDoubleArray* buildDoubleArray(const std::string& name, vtkIdType size)
{
	vtkDoubleArray* ret = vtkDoubleArray::New();
	ret->SetName(name.c_str());
	ret->Allocate(size);
	for (vtkIdType i = 0; i < size; ++i) {
		ret->InsertNextValue(0);
	}
	return ret;
}

} // namespace

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

	for (auto result : m_calculatedResults) {
		delete result;
	}
}

void v4PostZoneDataContainer::Impl::addCalculatedResultArrays()
{
	for (auto result : m_calculatedResults) {
		auto vals = buildDoubleArray(result->name(), result->dataSetAttributeValueCount());
		result->dataSetAttributes()->AddArray(vals);
		vals->Delete();
		result->updateValues();
	}
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
