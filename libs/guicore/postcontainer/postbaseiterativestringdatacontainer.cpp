#include "postbaseiterativestringdatacontainer.h"
#include "../misc/cgnsutil.h"

#include <cgnslib.h>

PostBaseIterativeStringDataContainer::PostBaseIterativeStringDataContainer(int baseId, const std::string& name, PostSolutionInfo* parent) :
	PostBaseIterativeDataContainer {baseId, name, parent}
{}

std::string PostBaseIterativeStringDataContainer::data() const
{
	return m_data;
}

bool PostBaseIterativeStringDataContainer::handleCurrentStepUpdate(const int fn, const int timeStep)
{
	int ier, nArrays;

	bool ok = getNumArrays(fn, &nArrays);
	if (! ok) {return false;}

	DataType_t dt;
	int dim;
	cgsize_t dimVec[3];
	int aid = CgnsUtil::findArray(name().c_str(), &dt, &dim, dimVec, nArrays);
	if (aid == 0) {return false;}

	std::vector<char> buffer;
	buffer.assign(dimVec[0] * dimVec[1], ' ');
	ier = cg_array_read(aid, buffer.data());

	std::vector<char> buffer2;
	buffer2.assign(dimVec[0] + 1, '\0');
	memcpy(buffer2.data(), buffer.data() + dimVec[0] * timeStep, dimVec[0]);
	int j = 0;
	while (j <= dimVec[0] && *(buffer2.data() + dimVec[0] - j) == ' ') {
		*(buffer2.data() + dimVec[0] - j) = '\0';
		++ j;
	}
	m_data = buffer2.data();
	return true;
}
