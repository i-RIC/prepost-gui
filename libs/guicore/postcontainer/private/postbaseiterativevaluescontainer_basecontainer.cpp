#include "postbaseiterativevaluescontainer_basecontainer.h"
#include "postbaseiterativevaluescontainer_integervaluecontainer.h"
#include "postbaseiterativevaluescontainer_realvaluecontainer.h"

#include <h5cgnsbaseiterativedata.h>

PostBaseIterativeValuesContainer::BaseContainer::BaseContainer() :
	BaseContainer(0)
{}

PostBaseIterativeValuesContainer::BaseContainer::BaseContainer(int baseId) :
	m_baseId {baseId}
{}

PostBaseIterativeValuesContainer::BaseContainer::~BaseContainer()
{
	clear();
}

int PostBaseIterativeValuesContainer::BaseContainer::baseId() const
{
	return m_baseId;
}

void PostBaseIterativeValuesContainer::BaseContainer::setBaseId(int baseId)
{
	m_baseId = baseId;
}

int PostBaseIterativeValuesContainer::BaseContainer::dataCount() const
{
	if (m_integerContainers.size() > 0) {
		return static_cast<int> (m_integerContainers.at(0)->values().size());
	} else if (m_realContainers.size() > 0) {
		return static_cast<int> (m_realContainers.at(0)->values().size());
	}
	return 0;
}

void PostBaseIterativeValuesContainer::BaseContainer::addContainer(IntegerValueContainer* container)
{
	m_integerContainers.push_back(container);
	m_integerNameMap.insert({container->name(), container});
}

void PostBaseIterativeValuesContainer::BaseContainer::addContainer(RealValueContainer* container)
{
	m_realContainers.push_back(container);
	m_realNameMap.insert({container->name(), container});
}

const std::vector<PostBaseIterativeValuesContainer::IntegerValueContainer*>& PostBaseIterativeValuesContainer::BaseContainer::integerContainers() const
{
	return m_integerContainers;
}

PostBaseIterativeValuesContainer::IntegerValueContainer* PostBaseIterativeValuesContainer::BaseContainer::integerContainer(const std::string& name) const
{
	auto it = m_integerNameMap.find(name);
	if (it == m_integerNameMap.end()) {return nullptr;}

	return it->second;
}

const std::vector<PostBaseIterativeValuesContainer::RealValueContainer*>& PostBaseIterativeValuesContainer::BaseContainer::realContainers() const
{
	return m_realContainers;
}

PostBaseIterativeValuesContainer::RealValueContainer* PostBaseIterativeValuesContainer::BaseContainer::realContainer(const std::string& name) const
{
	auto it = m_realNameMap.find(name);
	if (it == m_realNameMap.end()) {return nullptr;}

	return it->second;
}

PostBaseIterativeValuesContainer::ValueContainer* PostBaseIterativeValuesContainer::BaseContainer::container(const std::string& name) const
{
	auto icon = integerContainer(name);
	if (icon != nullptr) {return icon;}

	return realContainer(name);
}

void PostBaseIterativeValuesContainer::BaseContainer::setupContainers(iRICLib::H5CgnsBaseIterativeData* data)
{
	clear();

	std::vector<std::string> names;
	data->getResultNames(&names);

	for (auto name : names) {
		iRICLib::H5Util::DataArrayValueType type;
		data->readValueType(name, &type);
		if (type == iRICLib::H5Util::DataArrayValueType::Int) {
			auto c = new IntegerValueContainer(name);
			m_integerContainers.push_back(c);
			m_integerNameMap.insert({c->name(), c});
		} else if (type == iRICLib::H5Util::DataArrayValueType::RealDouble) {
			auto c = new RealValueContainer(name);
			m_realContainers.push_back(c);
			m_realNameMap.insert({c->name(), c});
		}
	}

	// try to read time
	std::vector<double> timeVals;
	data->readTime(&timeVals);
	if (timeVals.size() > 0) {
		auto c = new RealValueContainer("TimeValues");
		m_realContainers.push_back(c);
		m_realNameMap.insert({c->name(), c});
	}

	// try to read iteration
	std::vector<int> iterVals;
	data->readIteration(&iterVals);
	if (iterVals.size() > 0) {
		auto c = new IntegerValueContainer("IterationValues");
		m_integerContainers.push_back(c);
		m_integerNameMap.insert({c->name(), c});
	}
}

void PostBaseIterativeValuesContainer::BaseContainer::addValues(iRICLib::H5CgnsBaseIterativeData* data)
{
	for (auto c : m_integerContainers) {
		c->addValue(data);
	}
	for (auto c : m_realContainers) {
		c->addValue(data);
	}
}

void PostBaseIterativeValuesContainer::BaseContainer::clear()
{
	for (auto c : m_integerContainers) {
		delete c;
	}
	for (auto c : m_realContainers) {
		delete c;
	}

	m_integerContainers.clear();
	m_integerNameMap.clear();
	m_realContainers.clear();
	m_realNameMap.clear();
}
