#include "postdatacontainer.h"
#include "postsolutioninfo.h"

PostDataContainer::PostDataContainer(PostSolutionInfo *parent) :
	ProjectDataItem {parent}
{}

PostSolutionInfo* PostDataContainer::solutionInfo() const
{
	return dynamic_cast<PostSolutionInfo*>(parent());
}
