#include "../pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.h"
#include "../pre/gridcond/dimensionselectwidget/gridattributedimensiontimesliderselectwidget.h"
#include "solverdefinitiongridattributedimension.h"
#include "solverdefinition.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinitiongridattributedimension_impl.h"

#include <misc/stringtool.h>

#include <QDomElement>

SolverDefinitionGridAttributeDimension::Impl::Impl(const QDomElement& elem, SolverDefinition* solverDef, SolverDefinitionGridAttribute* att) :
	m_attribute {att}
{
	load(elem, solverDef);
}

void SolverDefinitionGridAttributeDimension::Impl::load(const QDomElement& elem, SolverDefinition* solverDef)
{
	auto translator = solverDef->buildTranslator();
	m_name = iRIC::toStr(elem.attribute("name"));
	m_englishCaption = iRIC::toStr(elem.attribute("caption"));
	m_caption = translator.translate(m_englishCaption.c_str());
}

SolverDefinitionGridAttributeDimension::SolverDefinitionGridAttributeDimension(const QDomElement& node, SolverDefinition* solverDef, SolverDefinitionGridAttribute* att) :
	SolverDefinitionNode {node, solverDef},
	impl {new Impl {node, solverDef, att}}
{}

SolverDefinitionGridAttributeDimension::~SolverDefinitionGridAttributeDimension()
{}

const std::string& SolverDefinitionGridAttributeDimension::name() const
{
	return impl->m_name;
}

const QString& SolverDefinitionGridAttributeDimension::caption() const
{
	return impl->m_caption;
}

const std::string& SolverDefinitionGridAttributeDimension::englishCaption() const
{
	return impl->m_englishCaption;
}

SolverDefinitionGridAttribute* SolverDefinitionGridAttributeDimension::attribute() const
{
	return impl->m_attribute;
}

GridAttributeDimensionSelectWidget* SolverDefinitionGridAttributeDimension::buildSelectWidget(GridAttributeDimensionContainer* container)
{
	if (impl->m_name == "Time") {
		return new GridAttributeDimensionTimeSliderSelectWidget(container, 0);
	} else {
		return new GridAttributeDimensionComboboxSelectWidget(container, 0);
	}
}
