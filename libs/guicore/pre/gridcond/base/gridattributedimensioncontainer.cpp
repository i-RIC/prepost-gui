#include "../../../solverdef/solverdefinitiongridattributedimension.h"
#include "gridattributedimensioncontainer.h"

#include <QVariant>

GridAttributeDimensionContainer::GridAttributeDimensionContainer(SolverDefinitionGridAttributeDimension* def) :
	GridAttributeBaseObject(def->attribute()) ,
	m_definition (def),
	m_currentIndex {0}
{}

GridAttributeDimensionContainer::~GridAttributeDimensionContainer()
{}

const std::string& GridAttributeDimensionContainer::name() const
{
	return m_definition->name();
}

const QString& GridAttributeDimensionContainer::caption() const
{
	return m_definition->caption();
}

SolverDefinitionGridAttributeDimension* GridAttributeDimensionContainer::definition() const
{
	return m_definition;
}

int GridAttributeDimensionContainer::currentIndex() const
{
	return m_currentIndex;
}

QVariant GridAttributeDimensionContainer::currentVariantValue() const
{
	return variantValue(currentIndex());
}

void GridAttributeDimensionContainer::setCurrentIndex(int index, bool noDraw)
{
	m_currentIndex = index;

	emit currentIndexChanged(noDraw);
	emit currentIndexChanged(index, noDraw);
}
