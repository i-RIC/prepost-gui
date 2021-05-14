#ifndef POSTBASEITERATIVEREALDATACONTAINER_H
#define POSTBASEITERATIVEREALDATACONTAINER_H

#include "postbaseiterativenumericaldatacontainer.h"

class PostBaseIterativeRealDataContainer : public PostBaseIterativeNumericalDataContainer
{
public:
	PostBaseIterativeRealDataContainer(const std::string& name, PostSolutionInfo* parent);

	double data() const override;
	bool handleCurrentStepUpdate(iRICLib::H5CgnsFile* file, const int timeStep) override;

private:
	double m_data;
};

#endif // POSTBASEITERATIVEREALDATACONTAINER_H
