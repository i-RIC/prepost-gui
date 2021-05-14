#include "postbaseiterativerealdatacontainer.h"

#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>
#include <h5cgnsfile.h>
#include <h5cgnsfilesolutionreader.h>
#include <iriclib_errorcodes.h>

PostBaseIterativeRealDataContainer::PostBaseIterativeRealDataContainer(const std::string& name, PostSolutionInfo* parent) :
	PostBaseIterativeNumericalDataContainer {name, parent}
{}

double PostBaseIterativeRealDataContainer::data() const
{
	return m_data;
}

bool PostBaseIterativeRealDataContainer::handleCurrentStepUpdate(iRICLib::H5CgnsFile* file, const int timeStep)
{
	int ier = file->ccBase()->biterData()->readValue(timeStep + 1, name(), &m_data);
	if (ier != IRIC_NO_ERROR) {return false;}

	return true;
}
