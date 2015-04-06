#include "solverdefinitionintegeroptionnodegridrelatedcondition.h"
#include "../pre/gridcond/editwidget/gridrelatedconditionintegeroptioneditwidget.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "../project/colorsource.h"

SolverDefinitionIntegerOptionNodeGridRelatedCondition::SolverDefinitionIntegerOptionNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
	: SolverDefinitionIntegerNodeGridRelatedCondition(node, translator)
{
	m_isOption = true;
	loadEnumeration(node, translator);
}

GridRelatedConditionEditWidget* SolverDefinitionIntegerOptionNodeGridRelatedCondition::editWidget(QWidget* parent)
{
	GridRelatedConditionIntegerOptionEditWidget* w = new GridRelatedConditionIntegerOptionEditWidget(parent, this);
	w->setEnumerations(m_enumerations);
	return w;
}

ScalarsToColorsEditWidget* SolverDefinitionIntegerOptionNodeGridRelatedCondition::createScalarsToColorsEditWidget(QWidget *parent)
{
	return createColorTransferFunctionEditWidget(parent);
}

ScalarsToColorsContainer* SolverDefinitionIntegerOptionNodeGridRelatedCondition::createScalarsToColorsContainer(ProjectDataItem* d)
{
	ColorTransferFunctionContainer* cont = createColorTransferFunctionContainer(d);
	QMap<double, QString> engEnums;
	QMap<double, QString> enums;
	QMap<int, QString>::iterator it;
	QMap<double, QColor> cols;
	ColorSource* src = new ColorSource(d);
	src->load(":/libs/guicore/data/colorsource_cell.xml");
	for (it = m_englishEnumerations.begin(); it != m_englishEnumerations.end(); ++it){
		int num = it.key();
		engEnums.insert(num, it.value());
		enums.insert(num, m_enumerations.value(num));
		cols.insert(num, src->getColor(num));
	}
	cont->setEnglishEnumerations(engEnums);
	cont->setEnumerations(enums);
	cont->setColors(cols);
	cont->setColors(cols);
	delete src;
	return cont;
}
