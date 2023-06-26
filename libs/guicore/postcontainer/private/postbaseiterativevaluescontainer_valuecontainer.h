#ifndef POSTBASEITERATIVEVALUESCONTAINER_VALUECONTAINER_H
#define POSTBASEITERATIVEVALUESCONTAINER_VALUECONTAINER_H

#include "../postbaseiterativevaluescontainer.h"

class PostBaseIterativeValuesContainer::ValueContainer
{
public:
	ValueContainer();
	ValueContainer(const std::string& name);
	virtual ~ValueContainer();

	const std::string& name() const;
	void setName(const std::string& name);

	virtual std::vector<double> doubleValues() const = 0;

protected:
	std::string m_name;
};

#endif // POSTBASEITERATIVEVALUESCONTAINER_VALUECONTAINER_H
