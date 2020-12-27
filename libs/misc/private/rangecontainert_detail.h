#ifndef RANGECONTAINERT_DETAIL_H
#define RANGECONTAINERT_DETAIL_H

#include "../rangecontainert.h"

template<typename T>
RangeContainerT<T>::RangeContainerT()
{}

template<typename T>
void RangeContainerT<T>::add(T data)
{
	if (m_data.size() == 0) {
		m_data.push_back(data);
		m_maxIndices.push_back(0);
		return;
	}

	auto lastData = m_data.back();
	if (lastData == data) {
		++ (m_maxIndices.back());
	} else {
		m_data.push_back(data);
		m_maxIndices.push_back(m_maxIndices.back() + 1);
	}
}

template<typename T>
const std::vector<T>& RangeContainerT<T>::data() const
{
	return m_data;
}

template<typename T>
const std::vector<std::size_t>& RangeContainerT<T>::maxIndices() const
{
	return m_maxIndices;
}

#endif // RANGECONTAINERT_DETAIL_H
