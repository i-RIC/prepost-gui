#ifndef POSTBASEITERATIVEVALUECONTAINER_VALUECONTAINERT_H
#define POSTBASEITERATIVEVALUECONTAINER_VALUECONTAINERT_H

#include "../postbaseiterativevaluescontainer.h"
#include "postbaseiterativevaluescontainer_valuecontainer.h"

#include <string>
#include <vector>

namespace iRICLib {

class H5CgnsBaseIterativeData;

} // namespace iRICLib

template<class V>
class PostBaseIterativeValuesContainer::ValueContainerT : public PostBaseIterativeValuesContainer::ValueContainer
{
public:
	ValueContainerT();
	ValueContainerT(const std::string& name);

	const std::vector<V>& values() const;
	std::vector<V>& values();
	std::vector<double> doubleValues() const override;

	void addValue(iRICLib::H5CgnsBaseIterativeData* data);
	void clear();

protected:
	std::vector<V> m_values;
};

#include "postbaseiterativevaluescontainer_valuecontainert_detail.h"

#endif // POSTBASEITERATIVEVALUECONTAINER_VALUECONTAINERT_H
