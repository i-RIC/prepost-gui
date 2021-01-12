#ifndef RANGECONTAINERT_H
#define RANGECONTAINERT_H

#include <cstddef>
#include <vector>

template<typename T>
class RangeContainerT
{
public:
	RangeContainerT();

	void add(T data);

	const std::vector<T>& data() const;
	const std::vector<std::size_t>& maxIndices() const;

private:
	std::vector<T> m_data;
	std::vector<std::size_t> m_maxIndices;
};

#include "private/rangecontainert_detail.h"

#endif // RANGECONTAINERT_H
