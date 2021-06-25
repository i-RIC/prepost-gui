#ifndef POSTBASEITERATIVESTRINGDATACONTAINER_H
#define POSTBASEITERATIVESTRINGDATACONTAINER_H

#include "postbaseiterativedatacontainer.h"

class PostBaseIterativeStringDataContainer : public PostBaseIterativeDataContainer
{
public:
	PostBaseIterativeStringDataContainer(const std::string& name, PostSolutionInfo* parent);
	std::string data() const;

	bool handleCurrentStepUpdate(iRICLib::H5CgnsFile* file, const int timeStep) override;

private:
	std::string m_data;
};

#endif // POSTBASEITERATIVESTRINGDATACONTAINER_H
