#ifndef POSTBASEITERATIVEINTEGERDATACONTAINER_H
#define POSTBASEITERATIVEINTEGERDATACONTAINER_H

#include "postbaseiterativenumericaldatacontainer.h"

class PostBaseIterativeIntegerDataContainer : public PostBaseIterativeNumericalDataContainer
{
public:
	PostBaseIterativeIntegerDataContainer(int baseId, const std::string& name, PostSolutionInfo* parent);

	double data() const override;
	bool handleCurrentStepUpdate(const int fn, const int timeStep) override;

private:
	int m_data;
};

#endif // POSTBASEITERATIVEINTEGERDATACONTAINER_H
