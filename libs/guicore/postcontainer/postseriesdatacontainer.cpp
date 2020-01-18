#include "../project/projectcgnsfile.h"
#include "postseriesdatacontainer.h"
#include "postsolutioninfo.h"

#include <misc/stringtool.h>

#include <QRegExp>

#include <cgnslib.h>
#include <iriclib.h>

PostSeriesDataContainer::PostSeriesDataContainer(PostSolutionInfo::Dimension dim, PostSolutionInfo* parent) :
	PostDataContainer {parent},
	m_dimension {dim},
	m_baseId {0},
	m_cellDim {0}
{}


const QList<double>& PostSeriesDataContainer::data() const
{
	return m_data;
}

bool PostSeriesDataContainer::handleCurrentStepUpdate(const int /*fn*/, const int /*timeStep*/)
{
	return true;
}

void PostSeriesDataContainer::update(const int fn)
{
	loadFromCgnsFile(fn);
}

bool PostSeriesDataContainer::setBaseId(const int fn)
{
	// if m_baseID is already set, we do not have to do it again.
	if (m_baseId != 0) {return true;}

	int ier;
	int numBases;
	int targetDim = PostSolutionInfo::toIntDimension(m_dimension);
	ier = cg_nbases(fn, &numBases);
	if (ier != 0) {return false;}
	for (int B = 1; B <= numBases; ++B) {
		char basename[32];
		int phys_dim;
		ier = cg_base_read(fn, B, basename, &m_cellDim, &phys_dim);
		if (ier != 0) {return false;}
		if (targetDim == m_cellDim) {
			m_baseId = B;
			return true;
		}
	}
	return false;
}

void PostSeriesDataContainer::loadFromCgnsFile(const int fn)
{
	bool ret;
	// set baseId.
	ret = setBaseId(fn);
	if (ret == false) {return;}
	ret = loadData(fn);
	emit dataUpdated();
}
