#ifndef POSTBASEITERATIVEVALUECONTAINER_VALUECONTAINERT_DETAIL_H
#define POSTBASEITERATIVEVALUECONTAINER_VALUECONTAINERT_DETAIL_H

#include "postbaseiterativevaluescontainer_valuecontainert.h"

#include <h5cgnsbaseiterativedata.h>

template<class V>
PostBaseIterativeValuesContainer::ValueContainerT<V>::ValueContainerT() :
	ValueContainer()
{}

template<class V>
PostBaseIterativeValuesContainer::ValueContainerT<V>::ValueContainerT(const std::string& name) :
	ValueContainer(name)
{}

template<class V>
const std::vector<V>& PostBaseIterativeValuesContainer::ValueContainerT<V>::values() const
{
	return m_values;
}

template<class V>
std::vector<V>& PostBaseIterativeValuesContainer::ValueContainerT<V>::values()
{
	return m_values;
}

template<class V>
std::vector<double> PostBaseIterativeValuesContainer::ValueContainerT<V>::doubleValues() const
{
	std::vector<double> vals;
	for (auto v : m_values) {
		vals.push_back(static_cast<double> (v));
	}
	return vals;
}

template<class V>
void PostBaseIterativeValuesContainer::ValueContainerT<V>::addValue(iRICLib::H5CgnsBaseIterativeData* data)
{
	V val;
	data->readValue(1, m_name, &val);

	m_values.push_back(val);
}

template<class V>
void PostBaseIterativeValuesContainer::ValueContainerT<V>::clear()
{
	m_values.clear();
}

#endif // POSTBASEITERATIVEVALUECONTAINER_VALUECONTAINERT_DETAIL_H
