#include "postbaseiterativevaluescontainer_realvaluecontainer.h"

PostBaseIterativeValuesContainer::RealValueContainer::RealValueContainer() :
	ValueContainerT<double> ()
{}

PostBaseIterativeValuesContainer::RealValueContainer::RealValueContainer(const std::string& name) :
	ValueContainerT<double> (name)
{}
