#ifndef POSTBASEITERATIVEVALUESCONTAINER_BASECONTAINER_H
#define POSTBASEITERATIVEVALUESCONTAINER_BASECONTAINER_H

#include "../../guicore_global.h"
#include "../postbaseiterativevaluescontainer.h"

#include <map>

namespace iRICLib {

class H5CgnsBaseIterativeData;

} // namespace iRICLib

class GUICOREDLL_EXPORT PostBaseIterativeValuesContainer::BaseContainer
{
public:
	BaseContainer();
	BaseContainer(int baseId);
	~BaseContainer();

	int baseId() const;
	void setBaseId(int baseId);

	int dataCount() const;

	void addContainer(IntegerValueContainer* container);
	void addContainer(RealValueContainer* container);

	const std::vector<IntegerValueContainer*>& integerContainers() const;
	IntegerValueContainer* integerContainer(const std::string& name) const;

	const std::vector<RealValueContainer*>& realContainers() const;
	RealValueContainer* realContainer(const std::string& name) const;

	ValueContainer* container(const std::string& name) const;

	void setupContainers(iRICLib::H5CgnsBaseIterativeData* data);
	void addValues(iRICLib::H5CgnsBaseIterativeData* data);

private:
	void clear();

	std::vector<IntegerValueContainer*> m_integerContainers;
	std::map<std::string, IntegerValueContainer*> m_integerNameMap;

	std::vector<RealValueContainer*> m_realContainers;
	std::map<std::string, RealValueContainer*> m_realNameMap;

	int m_baseId;
};

#endif // POSTBASEITERATIVEVALUESCONTAINER_BASECONTAINER_H
