#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridattributerealoptioncell.h"

SolverDefinitionGridAttributeRealOptionCell::SolverDefinitionGridAttributeRealOptionCell(QDomElement node, const SolverDefinitionTranslator& translator)
	: SolverDefinitionGridAttributeRealCell(node, translator)
{
	m_isOption = true;
	loadEnumeration(node, translator);
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionCell::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeRealOptionCell::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeRealOptionCell::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}
