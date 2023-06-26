#ifndef POSTBASEITERATIVEVALUECONTAINER_INTEGERVALUECONTAINER_H
#define POSTBASEITERATIVEVALUECONTAINER_INTEGERVALUECONTAINER_H

#include "postbaseiterativevaluescontainer_valuecontainert.h"
#include "../postbaseiterativevaluescontainer.h"

class PostBaseIterativeValuesContainer::IntegerValueContainer : public PostBaseIterativeValuesContainer::ValueContainerT<int>
{
public:
	IntegerValueContainer();
	IntegerValueContainer(const std::string& name);
};

#endif // POSTBASEITERATIVEVALUECONTAINER_INTEGERVALUECONTAINER_H
