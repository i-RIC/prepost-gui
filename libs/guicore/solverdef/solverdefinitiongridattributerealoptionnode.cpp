#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributerealoptionnode.h"

SolverDefinitionGridAttributeRealOptionNode::SolverDefinitionGridAttributeRealOptionNode(QDomElement node, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeRealNode {node, solverDef, true, order}
{
	loadEnumeration(node, solverDef->buildTranslator());
}

GridAttributeStringConverter* SolverDefinitionGridAttributeRealOptionNode::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionNode::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealOptionNode::variationEditWidget(QWidget*)
{
	return nullptr;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeRealOptionNode::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeRealOptionNode::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
