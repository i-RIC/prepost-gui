#include "../../../solverdef/solverdefinitiongridattributedimension.h"
#include "gridattributedimensioncontainer.h"

GridAttributeDimensionContainer::GridAttributeDimensionContainer(SolverDefinitionGridAttributeDimension* def)
	: GridAttributeBaseObject(def->attribute())
{
	m_definition = def;
	m_currentIndex = 0;
}

GridAttributeDimensionContainer::~GridAttributeDimensionContainer()
{

}

const QString& GridAttributeDimensionContainer::name() const
{
	return m_definition->name();
}

const QString& GridAttributeDimensionContainer::caption() const
{
	return m_definition->caption();
}

void GridAttributeDimensionContainer::setCurrentIndex(int index, bool noDraw)
{
	m_currentIndex = index;

	emit currentIndexChanged(noDraw);
	emit currentIndexChanged(index, noDraw);
}
