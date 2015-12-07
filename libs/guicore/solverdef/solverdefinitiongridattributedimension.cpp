#include "../pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.h"
#include "../pre/gridcond/dimensionselectwidget/gridattributedimensiontimesliderselectwidget.h"
#include "solverdefinitiongridattributedimension.h"
#include "solverdefinitiontranslator.h"
#include "private/solverdefinitiongridattributedimension_impl.h"

#include <misc/stringtool.h>

#include <QDomElement>

SolverDefinitionGridAttributeDimension::Impl::Impl(const QDomElement& elem, const SolverDefinitionTranslator &translator, SolverDefinitionGridAttribute* att) :
	m_attribute {att}
{
	load(elem, translator);
}

void SolverDefinitionGridAttributeDimension::Impl::load(const QDomElement& elem, const SolverDefinitionTranslator& translator)
{
	m_name = iRIC::toStr(elem.attribute("name"));
	m_englishCaption = iRIC::toStr(elem.attribute("caption"));
	m_caption = translator.translate(m_englishCaption.c_str());
}

SolverDefinitionGridAttributeDimension::SolverDefinitionGridAttributeDimension(const QDomElement& node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* att) :
	SolverDefinitionNode {node, translator},
	m_impl {new Impl {node, translator, att}}
{}

SolverDefinitionGridAttributeDimension::~SolverDefinitionGridAttributeDimension()
{
	delete m_impl;
}

const std::string& SolverDefinitionGridAttributeDimension::name() const
{
	return m_impl->m_name;
}

const QString& SolverDefinitionGridAttributeDimension::caption() const
{
	return m_impl->m_caption;
}

const std::string& SolverDefinitionGridAttributeDimension::englishCaption() const
{
	return m_impl->m_englishCaption;
}

SolverDefinitionGridAttribute* SolverDefinitionGridAttributeDimension::attribute() const
{
	return m_impl->m_attribute;
}

GridAttributeDimensionSelectWidget* SolverDefinitionGridAttributeDimension::buildSelectWidget(GridAttributeDimensionContainer* container)
{
	if (m_impl->m_name == "Time") {
		return new GridAttributeDimensionTimeSliderSelectWidget(container, 0);
	} else {
		return new GridAttributeDimensionComboboxSelectWidget(container, 0);
	}
}
