#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridattributeintegeroptionnode.h"

SolverDefinitionGridAttributeIntegerOptionNode::SolverDefinitionGridAttributeIntegerOptionNode(QDomElement node, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeIntegerNode {node, translator, true, order}
{
	loadEnumeration(node, translator);
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionNode::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
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
	for (auto it = m_englishEnumerations.begin(); it != m_englishEnumerations.end(); ++it) {
		int num = it.key();
		engEnums.insert(num, it.value());
		enums.insert(num, m_enumerations.value(num));
		cols.insert(num, src.getColor(num));
	}
	cont->setEnglishEnumerations(engEnums);
	cont->setEnumerations(enums);
	cont->setColors(cols);
	cont->setColors(cols);
	return cont;
}
