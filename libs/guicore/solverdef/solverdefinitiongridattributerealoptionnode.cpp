#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridattributerealoptionnode.h"

SolverDefinitionGridAttributeRealOptionNode::SolverDefinitionGridAttributeRealOptionNode(QDomElement node, const SolverDefinitionTranslator& translator) :
	SolverDefinitionGridAttributeRealNode {node, translator, true}
{
	loadEnumeration(node, translator);
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionNode::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeRealOptionNode::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeRealOptionNode::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
