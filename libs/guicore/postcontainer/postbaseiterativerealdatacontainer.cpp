#include "postbaseiterativerealdatacontainer.h"
#include "../misc/cgnsutil.h"

#include <cgnslib.h>

PostBaseIterativeRealDataContainer::PostBaseIterativeRealDataContainer(int baseId, const std::string& name, PostSolutionInfo* parent) :
	PostBaseIterativeNumericalDataContainer {baseId, name, parent}
{}

double PostBaseIterativeRealDataContainer::data() const
{
	return m_data;
}

bool PostBaseIterativeRealDataContainer::handleCurrentStepUpdate(const int fn, const int timeStep)
{
	int ier, nArrays;

	bool ok = getNumArrays(fn, &nArrays);
	if (! ok) {return false;}

	DataType_t dt;
	int dim;
	cgsize_t dimVec[3];
	int aid = CgnsUtil::findArray(name().c_str(), &dt, &dim, dimVec, nArrays);
	if (aid == 0) {return false;}

	std::vector<double> buffer;
	buffer.assign(dimVec[0], 0);
	ier = cg_array_read(aid, buffer.data());

	m_data = buffer.at(timeStep);
	return true;
}
