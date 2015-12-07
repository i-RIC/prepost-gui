#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridattributerealoptioncell.h"

SolverDefinitionGridAttributeRealOptionCell::SolverDefinitionGridAttributeRealOptionCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeRealCell {elem, translator, true, order}
{
	loadEnumeration(elem, translator);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealOptionCell::variationEditWidget(QWidget*)
{
	return nullptr;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionCell::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeRealOptionCell::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeRealOptionCell::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
