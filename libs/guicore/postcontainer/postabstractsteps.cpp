#include "../project/projectcgnsfile.h"
#include "postabstractsteps.h"

PostAbstractSteps::PostAbstractSteps(ProjectDataItem* parent) :
	ProjectDataItem(parent)
{}

void PostAbstractSteps::doLoadFromProjectMainFile(const QDomNode&)
{}

void PostAbstractSteps::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
