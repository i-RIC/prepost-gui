#include "solverdefinitionrealoptionnodegridrelatedcondition.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionrealoptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"

SolverDefinitionRealOptionNodeGridRelatedCondition::SolverDefinitionRealOptionNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
	: SolverDefinitionRealNodeGridRelatedCondition(node, translator)
{
	m_isOption = true;
	loadEnumeration(node, translator);
}

GridRelatedConditionEditWidget* SolverDefinitionRealOptionNodeGridRelatedCondition::editWidget(QWidget* parent)
{
	GridRelatedConditionRealOptionEditWidget* w = new GridRelatedConditionRealOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionRealOptionNodeGridRelatedCondition::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionRealOptionNodeGridRelatedCondition::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
