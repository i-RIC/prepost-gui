#ifndef POSTBASEITERATIVEVALUECONTAINER_REALVALUECONTAINER_H
#define POSTBASEITERATIVEVALUECONTAINER_REALVALUECONTAINER_H

#include "postbaseiterativevaluescontainer_valuecontainert.h"
#include "../postbaseiterativevaluescontainer.h"

class PostBaseIterativeValuesContainer::RealValueContainer : public PostBaseIterativeValuesContainer::ValueContainerT<double>
{
public:
	RealValueContainer();
	RealValueContainer(const std::string& name);
};

#endif // POSTBASEITERATIVEVALUECONTAINER_REALVALUECONTAINER_H
