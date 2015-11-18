#include "postdatacontainer.h"
#include "postsolutioninfo.h"

PostDataContainer::PostDataContainer(ProjectDataItem* parent) :
	ProjectDataItem {parent}
{}

bool PostDataContainer::handleCurrentStepUpdate(const int)
{
	return true;
}

PostSolutionInfo* PostDataContainer::solutionInfo()
{
	return dynamic_cast<PostSolutionInfo*>(parent());
}
