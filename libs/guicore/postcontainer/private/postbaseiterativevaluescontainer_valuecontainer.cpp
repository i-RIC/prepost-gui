#include "postbaseiterativevaluescontainer_valuecontainer.h"

PostBaseIterativeValuesContainer::ValueContainer::ValueContainer() :
	ValueContainer(std::string())
{}

PostBaseIterativeValuesContainer::ValueContainer::ValueContainer(const std::string& name) :
	m_name (name)
{}

PostBaseIterativeValuesContainer::ValueContainer::~ValueContainer()
{}

const std::string& PostBaseIterativeValuesContainer::ValueContainer::name() const
{
	return m_name;
}

void PostBaseIterativeValuesContainer::ValueContainer::setName(const std::string& name)
{
	m_name = name;
}
