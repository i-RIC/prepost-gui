#include "../project/projectcgnsfile.h"
#include "postbaseiterativeseriesdatacontainer.h"
#include "postsolutioninfo.h"

#include <misc/stringtool.h>

#include <cgnslib.h>
#include <vector>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

PostBaseIterativeSeriesDataContainer::PostBaseIterativeSeriesDataContainer(PostSolutionInfo::Dimension dim, const QString& biName, PostSolutionInfo* parent) :
	PostSeriesDataContainer {dim, parent},
	m_baseIterativeName {biName}
{}

bool PostBaseIterativeSeriesDataContainer::loadData(const int fn)
{
	int ier, nSteps;
	char iterName[32];
	m_data.clear();

	ier = cg_biter_read(fn, m_baseId, iterName, &nSteps);
	if (ier != 0) {return false;}
	ier = cg_goto(fn, m_baseId, iterName, 0, "end");
	if (ier != 0) {return false;}
	int numArrays;
	cg_narrays(&numArrays);
	for (int i = 1; i <= numArrays; ++i) {
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		char arrayname[30];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		if (m_baseIterativeName == arrayname) {
			// load data.
			// it must be an array with dimension 1.
			std::vector<double> buffer(dimVector[0], 0);
			ier = cg_array_read_as(i, RealDouble, buffer.data());
			if (ier != 0) {return false;}
			for (double d : buffer) {
				m_data.append(d);
			}
			return true;
		}
	}
	return false;
}

void PostBaseIterativeSeriesDataContainer::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PostBaseIterativeSeriesDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
