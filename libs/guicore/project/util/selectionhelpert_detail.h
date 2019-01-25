#ifndef SELECTIONHELPERT_DETAIL_H
#define SELECTIONHELPERT_DETAIL_H

#include "selectionhelpert_impl.h"

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>::Impl::Impl()
{}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>::SelectionHelperT() :
	impl {new Impl()}
{}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>::~SelectionHelperT()
{}

template <typename VIEW, typename VIEWHELPER>
const std::vector<std::unique_ptr<VIEWHELPER>>& SelectionHelperT<VIEW, VIEWHELPER>::viewHelpers() const
{
	return impl->m_viewHelpers;
}

template <typename VIEW, typename VIEWHELPER>
std::vector<std::unique_ptr<VIEWHELPER>>& SelectionHelperT<VIEW, VIEWHELPER>::viewHelpers()
{
	return impl->m_viewHelpers;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::keyPressEvent(QKeyEvent*, VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::keyReleaseEvent(QKeyEvent*, VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::mouseDoubleClickEvent(QMouseEvent* , VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::mouseMoveEvent(QMouseEvent* , VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::mousePressEvent(QMouseEvent* , VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::mouseReleaseEvent(QMouseEvent* , VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
SelectionHelperT<VIEW, VIEWHELPER>* SelectionHelperT<VIEW, VIEWHELPER>::wheelEvent(QWheelEvent* , VIEW*)
{
	return this;
}

template <typename VIEW, typename VIEWHELPER>
void SelectionHelperT<VIEW, VIEWHELPER>::finishSelection(VIEW* view)
{
	doFinishSelection(view);
	view->updateView();
}

template <typename VIEW, typename VIEWHELPER>
void SelectionHelperT<VIEW, VIEWHELPER>::doFinishSelection(VIEW*)
{}

#endif // SELECTIONHELPERT_DETAIL_H
