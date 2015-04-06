#include "gridrelatedconditiondimensioncontainer.h"
#include "../../../solverdef/solverdefinitiongridrelatedconditiondimension.h"

GridRelatedConditionDimensionContainer::GridRelatedConditionDimensionContainer(SolverDefinitionGridRelatedConditionDimension* def)
	: GridRelatedConditionBaseObject(def->condition())
{
	m_definition = def;
	m_currentIndex = 0;
}

GridRelatedConditionDimensionContainer::~GridRelatedConditionDimensionContainer()
{

}

const QString& GridRelatedConditionDimensionContainer::name() const
{
	return m_definition->name();
}

const QString& GridRelatedConditionDimensionContainer::caption() const
{
	return m_definition->caption();
}

void GridRelatedConditionDimensionContainer::setCurrentIndex(int index, bool noDraw)
{
	m_currentIndex = index;

	emit currentIndexChanged(noDraw);
	emit currentIndexChanged(index, noDraw);
}
