#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroptioncell.h"

#include <vtkColorTransferFunction.h>

SolverDefinitionGridAttributeIntegerOptionCell::SolverDefinitionGridAttributeIntegerOptionCell(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeIntegerCell(elem, solverDef, true, order)
{
	loadEnumeration(elem, solverDef->buildTranslator());
}

GridAttributeStringConverter* SolverDefinitionGridAttributeIntegerOptionCell::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionCell::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOptionCell::variationEditWidget(QWidget*)
{
	return nullptr;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeIntegerOptionCell::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeIntegerOptionCell::createScalarsToColorsContainer(ProjectDataItem* d)
{
	ColorTransferFunctionContainer* cont = createColorTransferFunctionContainer(d);
	QMap<double, QString> engEnums;
	QMap<double, QString> enums;
	QMap<double, QColor> cols;
	ColorSource src(d);
	src.load(":/libs/guicore/data/colorsource_cell.xml");
	for (auto it = englishEnumerations().begin(); it != englishEnumerations().end(); ++it) {
		int num = it.key();
		engEnums.insert(num, it.value().c_str());
		enums.insert(num, enumerations().value(num));
		cols.insert(num, src.getColor(num));
	}
	cont->setEnglishEnumerations(engEnums);
	cont->setEnumerations(enums);
	cont->setColors(cols);
	cont->setColors(cols);
	return cont;
}
