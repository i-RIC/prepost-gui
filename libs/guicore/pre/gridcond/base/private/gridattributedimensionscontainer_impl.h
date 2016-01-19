#ifndef GRIDATTRIBUTEDIMENSIONSCONTAINER_IMPL_H
#define GRIDATTRIBUTEDIMENSIONSCONTAINER_IMPL_H

#include "../gridattributedimensionscontainer.h"

class GridAttributeDimensionsContainer::Impl
{
public:
	int m_currentIndex;
	std::vector<GridAttributeDimensionContainer*> m_containers;
	std::vector<GridAttributeDimensionSelectWidget*> m_selectWidgets;
};

#endif // GRIDATTRIBUTEDIMENSIONSCONTAINER_IMPL_H

