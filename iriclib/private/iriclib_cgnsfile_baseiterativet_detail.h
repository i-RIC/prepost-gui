#ifndef IRICLIB_CGNSFILE_BASEITERATIVET_DETAIL_H
#define IRICLIB_CGNSFILE_BASEITERATIVET_DETAIL_H

#include "iriclib_cgnsfile_baseiterativet.h"

template <typename V>
BaseIterativeT<V>::BaseIterativeT(const std::string& name) :
	m_name (name)
{}

template <typename V>
std::string BaseIterativeT<V>::name() const
{
	return m_name;
}

template <typename V>
const std::vector<V>& BaseIterativeT<V>::values() const
{
	return m_values;
}

template <typename V>
void BaseIterativeT<V>::setValues(const std::vector<V>& vals)
{
	m_values = vals;
}

template <typename V>
void BaseIterativeT<V>::addValue(V newVal)
{
	m_values.push_back(newVal);
}

#endif // IRICLIB_CGNSFILE_BASEITERATIVET_DETAIL_H
