#ifndef SELECTIONHELPERT_IMPL_H
#define SELECTIONHELPERT_IMPL_H

#include "selectionhelpert.h"

template <typename VIEW, typename VIEWHELPER>
class SelectionHelperT<VIEW, VIEWHELPER>::Impl
{
public:
	Impl();

	std::vector<std::unique_ptr<VIEWHELPER>> m_viewHelpers;
};

#endif // SELECTIONHELPERT_IMPL_H
