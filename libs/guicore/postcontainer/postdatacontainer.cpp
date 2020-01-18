#include "postdatacontainer.h"
#include "postsolutioninfo.h"

PostDataContainer::PostDataContainer(PostSolutionInfo *parent) :
	ProjectDataItem {parent}
{}

bool PostDataContainer::handleCurrentStepUpdate(const int /*fn*/, const int /*timeStep*/)
{
	return true;
}

PostSolutionInfo* PostDataContainer::solutionInfo() const
{
	return dynamic_cast<PostSolutionInfo*>(parent());
}
