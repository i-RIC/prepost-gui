#include "solverdefinitionrealoptioncellgridrelatedcondition.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionrealoptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"

SolverDefinitionRealOptionCellGridRelatedCondition::SolverDefinitionRealOptionCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
	: SolverDefinitionRealCellGridRelatedCondition(node, translator)
{
	m_isOption = true;
	loadEnumeration(node, translator);
}

GridRelatedConditionEditWidget* SolverDefinitionRealOptionCellGridRelatedCondition::editWidget(QWidget* parent)
{
	GridRelatedConditionRealOptionEditWidget* w = new GridRelatedConditionRealOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionRealOptionCellGridRelatedCondition::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionRealOptionCellGridRelatedCondition::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
