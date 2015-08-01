#include "postdatacontainer.h"
#include "postsolutioninfo.h"

PostDataContainer::PostDataContainer(ProjectDataItem* parent) :
	ProjectDataItem {parent}
{}

PostSolutionInfo* PostDataContainer::solutionInfo()
{
	return dynamic_cast<PostSolutionInfo*>(parent());
}
