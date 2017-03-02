#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroptionnode.h"

SolverDefinitionGridAttributeIntegerOptionNode::SolverDefinitionGridAttributeIntegerOptionNode(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeIntegerNode {elem, solverDef, true, order}
{
	loadEnumeration(elem, solverDef->buildTranslator());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionNode::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOptionNode::variationEditWidget(QWidget*)
{
	return nullptr;
}

ScalarsToColorsEditWidget* SolverDefinitionGridAttributeIntegerOptionNode::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionGridAttributeIntegerOptionNode::createScalarsToColorsContainer(ProjectDataItem* d)
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
