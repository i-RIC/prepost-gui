#include "postbaseiterativedatacontainer.h"

#include <cgnslib.h>

PostBaseIterativeDataContainer::PostBaseIterativeDataContainer(int baseId, const std::string& name, PostSolutionInfo* parent) :
	PostDataContainer {parent},
	m_baseId {baseId},
	m_name (name)
{}

int PostBaseIterativeDataContainer::baseId() const
{
	return m_baseId;
}

const std::string& PostBaseIterativeDataContainer::name() const
{
	return m_name;
}

bool PostBaseIterativeDataContainer::getNumArrays(const int fn, int* nArrays)
{
	int ier, nSteps;
	char iterName[32];

	ier = cg_biter_read(fn, baseId(), iterName, &nSteps);
	if (ier != 0) {return false;}
	ier = cg_goto(fn, baseId(), iterName, 0, nullptr);
	if (ier != 0) {return false;}
	ier = cg_narrays(nArrays);
	if (ier != 0) {return false;}

	return true;
}

void PostBaseIterativeDataContainer::doLoadFromProjectMainFile(const QDomNode& node)
{}

void PostBaseIterativeDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{}
