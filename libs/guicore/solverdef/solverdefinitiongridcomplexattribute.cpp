#include "../pre/gridcond/complex/gridcomplexattributecontainer.h"
#include "../pre/gridcond/complex/gridcomplexattributeeditwidget.h"
#include "../pre/rawdatabackground/rawdatabackgroundcomplexcreator.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridcomplexattribute.h"

#include <misc/xmlsupport.h>

void SolverDefinitionGridComplexAttribute::load(const QDomElement node)
{
	QDomElement defNode = iRIC::getChildNode(node, "Definition").toElement();
	if (defNode.attribute("position") == "cell") {
		m_position = CellCenter;
	}
	m_element = defNode;
}

GridAttributeContainer* SolverDefinitionGridComplexAttribute::container(Grid* grid)
{
	return buildContainer(grid);
}

GridAttributeContainer* SolverDefinitionGridComplexAttribute::buildContainer(Grid* grid)
{
	return new GridComplexAttributeContainer(grid, this);
}

GridAttributeEditWidget* SolverDefinitionGridComplexAttribute::editWidget(QWidget* parent)
{
	return new GridComplexAttributeEditWidget(parent, this);
}

ScalarsToColorsEditWidget* SolverDefinitionGridComplexAttribute::createScalarsToColorsEditWidget(QWidget* parent)
{
	return createColorTransferFunctionEditWidget(parent);
}

QString SolverDefinitionGridComplexAttribute::undefinedString()
{
	return QObject::tr("(Undefined)");
}

QString SolverDefinitionGridComplexAttribute::undefinedEnglishString()
{
	return "(Undefined)";
}

RawData* SolverDefinitionGridComplexAttribute::buildBackgroundRawData(ProjectDataItem* parent)
{
	return RawDataBackgroundComplexCreator::instance()->create(parent, this);
}

ScalarsToColorsContainer* SolverDefinitionGridComplexAttribute::createScalarsToColorsContainer(ProjectDataItem* d)
{
	ColorTransferFunctionContainer* cont = createColorTransferFunctionContainer(d);
	return cont;
}
