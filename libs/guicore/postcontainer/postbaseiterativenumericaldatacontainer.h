#ifndef POSTBASEITERATIVENUMERICALDATACONTAINER_H
#define POSTBASEITERATIVENUMERICALDATACONTAINER_H

#include "postbaseiterativedatacontainer.h"

class PostBaseIterativeNumericalDataContainer : public PostBaseIterativeDataContainer
{
public:
	PostBaseIterativeNumericalDataContainer(const std::string& name, PostSolutionInfo* parent);

	virtual double data() const = 0;
};

#endif // POSTBASEITERATIVENUMERICALDATACONTAINER_H
