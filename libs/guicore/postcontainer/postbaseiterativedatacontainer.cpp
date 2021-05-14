#include "postbaseiterativedatacontainer.h"
#include "../project/projectcgnsfile.h"
#include <cgnslib.h>

PostBaseIterativeDataContainer::PostBaseIterativeDataContainer(const std::string& name, PostSolutionInfo* parent) :
	PostDataContainer {parent},
	m_name (name)
{}

const std::string& PostBaseIterativeDataContainer::name() const
{
	return m_name;
}

void PostBaseIterativeDataContainer::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PostBaseIterativeDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
