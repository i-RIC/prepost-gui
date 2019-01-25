#ifndef SELECTIONHELPERT_H
#define SELECTIONHELPERT_H

#include "selectionhelper.h"

#include <memory>
#include <vector>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

template <typename VIEW, typename VIEWHELPER>
class SelectionHelperT : public SelectionHelper
{
public:
	SelectionHelperT();
	virtual ~SelectionHelperT();

	virtual const std::vector<std::unique_ptr<VIEWHELPER>>& viewHelpers() const;
	virtual std::vector<std::unique_ptr<VIEWHELPER>>& viewHelpers();

	virtual SelectionHelperT* keyPressEvent(QKeyEvent* event, VIEW* view);
	virtual SelectionHelperT* keyReleaseEvent(QKeyEvent* event, VIEW* view);
	virtual SelectionHelperT* mouseDoubleClickEvent(QMouseEvent* event, VIEW* view);
	virtual SelectionHelperT* mouseMoveEvent(QMouseEvent* event, VIEW* view);
	virtual SelectionHelperT* mousePressEvent(QMouseEvent* event, VIEW* view);
	virtual SelectionHelperT* mouseReleaseEvent(QMouseEvent* event, VIEW* view);
	virtual SelectionHelperT* wheelEvent(QWheelEvent* event, VIEW* view);

protected:
	virtual void finishSelection(VIEW* view);
	virtual void doFinishSelection(VIEW* view);

private:
	class Impl;
	Impl* impl;
};

#include "selectionhelpert_detail.h"

#endif // SELECTIONHELPERT_H
