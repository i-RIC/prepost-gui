#include "../pre/gridcond/complex/gridrelatedcomplexconditioncontainer.h"
#include "../pre/gridcond/complex/gridrelatedcomplexconditioneditwidget.h"
#include "../pre/rawdatabackground/rawdatabackgroundcomplexcreator.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridrelatedcomplexcondition.h"

#include <misc/xmlsupport.h>

void SolverDefinitionGridRelatedComplexCondition::load(const QDomElement node)
{
	QDomElement defNode = iRIC::getChildNode(node, "Definition").toElement();
	if (defNode.attribute("position") == "cell") {
		m_position = CellCenter;
	}
	m_element = defNode;
}

GridRelatedConditionContainer* SolverDefinitionGridRelatedComplexCondition::container(Grid* grid)
{
	return buildContainer(grid);
}

GridRelatedConditionContainer* SolverDefinitionGridRelatedComplexCondition::buildContainer(Grid* grid)
{
	return new GridRelatedComplexConditionContainer(grid, this);
}

GridRelatedConditionEditWidget* SolverDefinitionGridRelatedComplexCondition::editWidget(QWidget* parent)
{
	return new GridRelatedComplexConditionEditWidget(parent, this);
}

ScalarsToColorsEditWidget* SolverDefinitionGridRelatedComplexCondition::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createColorTransferFunctionEditWidget(parent);
}

QString SolverDefinitionGridRelatedComplexCondition::undefinedString()
{
	return QObject::tr("(Undefined)");
}

QString SolverDefinitionGridRelatedComplexCondition::undefinedEnglishString()
{
	return "(Undefined)";
}

RawData* SolverDefinitionGridRelatedComplexCondition::buildBackgroundRawData(ProjectDataItem* parent)
{
	return RawDataBackgroundComplexCreator::instance()->create(parent, this);
}

ScalarsToColorsContainer* SolverDefinitionGridRelatedComplexCondition::createScalarsToColorsContainer(ProjectDataItem* d)
{
	ColorTransferFunctionContainer* cont = createColorTransferFunctionContainer(d);
	return cont;
}
