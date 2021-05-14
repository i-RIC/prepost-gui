#ifndef POSTBASEITERATIVEINTEGERDATACONTAINER_H
#define POSTBASEITERATIVEINTEGERDATACONTAINER_H

#include "postbaseiterativenumericaldatacontainer.h"

class PostBaseIterativeIntegerDataContainer : public PostBaseIterativeNumericalDataContainer
{
public:
	PostBaseIterativeIntegerDataContainer(const std::string& name, PostSolutionInfo* parent);

	double data() const override;
	bool handleCurrentStepUpdate(iRICLib::H5CgnsFile* file, const int timeStep) override;

private:
	int m_data;
};

#endif // POSTBASEITERATIVEINTEGERDATACONTAINER_H
