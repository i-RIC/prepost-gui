#include "valuerangecontainerutil.h"
#include "vtkpointsetvaluerangeset.h"

#include <guibase/vtkdatasetattributestool.h>

#include <vtkCellData.h>
#include <vtkPointData.h>

vtkPointSetValueRangeSet::vtkPointSetValueRangeSet()
{}

const ValueRangeContainer& vtkPointSetValueRangeSet::pointDataValueRange(const std::string& name) const
{
	return m_pointDataValueRanges.find(name)->second;
}

const std::unordered_map<std::string, ValueRangeContainer>& vtkPointSetValueRangeSet::pointDataValueRanges() const
{
	return m_pointDataValueRanges;
}

const ValueRangeContainer& vtkPointSetValueRangeSet::cellDataValueRange(const std::string& name) const
{
	return m_cellDataValueRanges.find(name)->second;
}

const std::unordered_map<std::string, ValueRangeContainer>& vtkPointSetValueRangeSet::cellDataValueRanges() const
{
	return m_cellDataValueRanges;
}

void vtkPointSetValueRangeSet::update(vtkPointSet* data)
{
	m_pointDataValueRanges.clear();
	m_cellDataValueRanges.clear();

	auto pd = data->GetPointData();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
		auto data = pd->GetArray(name.c_str());
		auto range = ValueRangeContainerUtil::build(data);
		m_pointDataValueRanges.insert({name, range});
	}
	auto cd = data->GetCellData();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd)) {
		auto data = cd->GetArray(name.c_str());
		auto range = ValueRangeContainerUtil::build(data);
		m_cellDataValueRanges.insert({name, range});
	}
}
