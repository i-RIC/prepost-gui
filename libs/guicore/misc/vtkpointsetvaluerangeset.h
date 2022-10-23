#ifndef VTKPOINTSETVALUERANGESET_H
#define VTKPOINTSETVALUERANGESET_H

#include "../guicore_global.h"
#include "valuerangecontainer.h"

#include <string>
#include <unordered_map>

class vtkPointSet;

class GUICOREDLL_EXPORT vtkPointSetValueRangeSet
{
public:
	vtkPointSetValueRangeSet();

	const ValueRangeContainer& pointDataValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& pointDataValueRanges() const;

	const ValueRangeContainer& cellDataValueRange(const std::string& name) const;
	const std::unordered_map<std::string, ValueRangeContainer>& cellDataValueRanges() const;

	void update(vtkPointSet* data);

private:
	std::unordered_map<std::string, ValueRangeContainer> m_pointDataValueRanges;
	std::unordered_map<std::string, ValueRangeContainer> m_cellDataValueRanges;
};

#endif // VTKPOINTSETVALUERANGESET_H
