#include "postbaseiterativeintegerdatacontainer.h"
#include "../misc/cgnsutil.h"

#include <h5cgnsbase.h>
#include <h5cgnsbaseiterativedata.h>
#include <h5cgnsfile.h>
#include <h5cgnsfilesolutionreader.h>
#include <iriclib_errorcodes.h>

PostBaseIterativeIntegerDataContainer::PostBaseIterativeIntegerDataContainer(const std::string& name, PostSolutionInfo* parent) :
	PostBaseIterativeNumericalDataContainer {name, parent}
{}

double PostBaseIterativeIntegerDataContainer::data() const
{
	return m_data;
}

bool PostBaseIterativeIntegerDataContainer::handleCurrentStepUpdate(iRICLib::H5CgnsFile* file, const int timeStep)
{
	int ier = file->ccBase()->biterData()->readValue(timeStep + 1, name(), &m_data);
	if (ier != IRIC_NO_ERROR) {return false;}

	return true;
}
