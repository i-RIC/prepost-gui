#include "../../../solverdef/solverdefinitiongridattributedimension.h"
#include "gridattributedimensioncontainer.h"
#include "private/gridattributedimensioncontainer_impl.h"

#include <QVariant>

GridAttributeDimensionContainer::Impl::Impl(SolverDefinitionGridAttributeDimension* def) :
	m_definition {def},
	m_currentIndex {0}
{}

GridAttributeDimensionContainer::GridAttributeDimensionContainer(SolverDefinitionGridAttributeDimension* def) :
	GridAttributeBaseObject(def->attribute()),
	impl {new Impl {def}}
{}

GridAttributeDimensionContainer::~GridAttributeDimensionContainer()
{
	delete impl;
}

std::string GridAttributeDimensionContainer::name() const
{
	return impl->m_definition->name();
}

QString GridAttributeDimensionContainer::caption() const
{
	return impl->m_definition->caption();
}

SolverDefinitionGridAttributeDimension* GridAttributeDimensionContainer::definition() const
{
	return impl->m_definition;
}

int GridAttributeDimensionContainer::currentIndex() const
{
	return impl->m_currentIndex;
}

QVariant GridAttributeDimensionContainer::currentVariantValue() const
{
	return variantValue(currentIndex());
}

void GridAttributeDimensionContainer::setCurrentIndex(int index, bool noDraw)
{
	impl->m_currentIndex = index;

	emit currentIndexChanged(noDraw);
	emit currentIndexChanged(index, noDraw);
}
