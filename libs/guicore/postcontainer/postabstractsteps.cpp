#include "../project/projectcgnsfile.h"
#include "postabstractsteps.h"
#include "postsolutioninfo.h"

PostAbstractSteps::PostAbstractSteps(PostSolutionInfo* parent) :
	ProjectDataItem(parent)
{}

PostSolutionInfo* PostAbstractSteps::postSolutionInfo() const
{
	return dynamic_cast<PostSolutionInfo*> (parent());
}

void PostAbstractSteps::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PostAbstractSteps::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
