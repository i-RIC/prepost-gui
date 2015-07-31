#include "../pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.h"
#include "../pre/gridcond/dimensionselectwidget/gridattributedimensiontimesliderselectwidget.h"
#include "solverdefinitiongridattributedimension.h"
#include "solverdefinitiontranslator.h"

class SolverDefinitionGridAttributeDimension::Impl
{
public:
	Impl(const QDomElement& node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* att);
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);

	QString m_name;
	QString m_caption;
	QString m_englishCaption;

	SolverDefinitionGridAttribute* m_attribute;
};

SolverDefinitionGridAttributeDimension::Impl::Impl(const QDomElement &node, const SolverDefinitionTranslator &translator, SolverDefinitionGridAttribute* att) :
	m_attribute {att}
{
	load(node, translator);
}

void SolverDefinitionGridAttributeDimension::Impl::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	m_name = node.attribute("name");
	m_englishCaption = node.attribute("caption");
	m_caption = translator.translate(m_englishCaption);
}

SolverDefinitionGridAttributeDimension::SolverDefinitionGridAttributeDimension(const QDomElement& node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* att) :
	SolverDefinitionNode {node, translator},
	m_impl {new Impl {node, translator, att}}
{}

SolverDefinitionGridAttributeDimension::~SolverDefinitionGridAttributeDimension()
{
	delete m_impl;
}

const QString& SolverDefinitionGridAttributeDimension::name() const
{
	return m_impl->m_name;
}

const QString& SolverDefinitionGridAttributeDimension::caption() const
{
	return m_impl->m_caption;
}

const QString& SolverDefinitionGridAttributeDimension::englishCaption() const
{
	return m_impl->m_englishCaption;
}

SolverDefinitionGridAttribute* SolverDefinitionGridAttributeDimension::condition() const
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
