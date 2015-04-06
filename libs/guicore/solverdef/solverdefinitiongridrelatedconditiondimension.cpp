#include "solverdefinitiongridrelatedconditiondimension.h"
#include "../pre/gridcond/dimensionselectwidget/gridrelatedconditiondimensioncomboboxselectwidget.h"
#include "../pre/gridcond/dimensionselectwidget/gridrelatedconditiondimensiontimesliderselectwidget.h"

SolverDefinitionGridRelatedConditionDimension::~SolverDefinitionGridRelatedConditionDimension()
{

}

void SolverDefinitionGridRelatedConditionDimension::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	m_name = node.attribute("name");
	m_englishCaption = node.attribute("caption");
	m_caption = translator.translate(m_englishCaption);
}


GridRelatedConditionDimensionSelectWidget* SolverDefinitionGridRelatedConditionDimension::buildSelectWidget(GridRelatedConditionDimensionContainer* container)
{
	if (m_name == "Time"){
		return new GridRelatedConditionDimensionTimeSliderSelectWidget(container, 0);
	} else {
		return new GridRelatedConditionDimensionComboboxSelectWidget(container, 0);
	}
}
