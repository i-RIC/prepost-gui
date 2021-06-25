#include "../project/projectcgnsfile.h"
#include "postseriesdatacontainer.h"
#include "postsolutioninfo.h"

#include <misc/stringtool.h>

#include <QRegExp>

PostSeriesDataContainer::PostSeriesDataContainer(PostSolutionInfo::Dimension dim, PostSolutionInfo* parent) :
	PostDataContainer {parent},
	m_dimension {dim}
{}


const std::vector<double>& PostSeriesDataContainer::data() const
{
	return m_data;
}

int PostSeriesDataContainer::update()
{
	return loadFromCgnsFile();
}

int PostSeriesDataContainer::loadFromCgnsFile()
{
	int ier = loadData();
	emit dataUpdated();

	return ier;
}
