#ifndef POSTBASEITERATIVEREALDATACONTAINER_H
#define POSTBASEITERATIVEREALDATACONTAINER_H

#include "postbaseiterativenumericaldatacontainer.h"

class PostBaseIterativeRealDataContainer : public PostBaseIterativeNumericalDataContainer
{
public:
	PostBaseIterativeRealDataContainer(int baseId, const std::string& name, PostSolutionInfo* parent);

	double data() const override;
	bool handleCurrentStepUpdate(const int fn, const int timeStep) override;

private:
	double m_data;
};

#endif // POSTBASEITERATIVEREALDATACONTAINER_H
