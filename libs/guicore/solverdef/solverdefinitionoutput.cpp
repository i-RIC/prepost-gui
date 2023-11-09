#include "solverdefinitionoutput.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinitionoutput_impl.h"

#include <misc/stringtool.h>

SolverDefinitionOutput::SolverDefinitionOutput(const std::string name) :
	impl {new Impl {}}
{
	impl->m_name = name;
	impl->m_englishCaption = name;

	impl->m_caption = name.c_str();
}

SolverDefinitionOutput::SolverDefinitionOutput(QDomElement elem, SolverDefinition* solverDef, const SolverDefinitionTranslator& translator) :
	SolverDefinitionNode(elem, solverDef),
	impl {new Impl {}}
{
	impl->m_name = iRIC::toStr(elem.attribute("name"));
	impl->m_englishCaption = iRIC::toStr(elem.attribute("caption"));

	impl->m_caption = translator.translate(impl->m_englishCaption.c_str());
}

SolverDefinitionOutput::~SolverDefinitionOutput()
{}

const std::string& SolverDefinitionOutput::name() const
{
	return impl->m_name;
}

const QString& SolverDefinitionOutput::caption() const
{
	return impl->m_caption;
}

const std::string& SolverDefinitionOutput::englishCaption() const
{
	return impl->m_englishCaption;
}
