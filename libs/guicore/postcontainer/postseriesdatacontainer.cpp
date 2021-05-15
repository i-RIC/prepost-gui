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
	return true;
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
