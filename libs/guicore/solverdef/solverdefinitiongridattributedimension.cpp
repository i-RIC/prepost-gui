#include "../pre/gridcond/dimensionselectwidget/gridattributedimensioncomboboxselectwidget.h"
#include "../pre/gridcond/dimensionselectwidget/gridattributedimensiontimesliderselectwidget.h"
#include "solverdefinitiongridattributedimension.h"

SolverDefinitionGridAttributeDimension::~SolverDefinitionGridAttributeDimension()
{

}

void SolverDefinitionGridAttributeDimension::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	m_name = node.attribute("name");
	m_englishCaption = node.attribute("caption");
	m_caption = translator.translate(m_englishCaption);
}


GridAttributeDimensionSelectWidget* SolverDefinitionGridAttributeDimension::buildSelectWidget(GridAttributeDimensionContainer* container)
{
	if (m_name == "Time") {
		return new GridAttributeDimensionTimeSliderSelectWidget(container, 0);
	} else {
		return new GridAttributeDimensionComboboxSelectWidget(container, 0);
	}
}
