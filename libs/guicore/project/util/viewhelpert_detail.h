#ifndef VIEWHELPERT_DETAIL_H
#define VIEWHELPERT_DETAIL_H

template <typename VIEW>
ViewHelperT<VIEW>::ViewHelperT(VIEW* view) :
	m_view {view},
	m_pen {QPen {}},
	m_brush {QBrush()}
{}

template <typename VIEW>
ViewHelperT<VIEW>::ViewHelperT(VIEW* view, const QPen& pen) :
	m_view {view},
	m_pen {pen},
	m_brush {QBrush()}
{}

template <typename VIEW>
ViewHelperT<VIEW>::ViewHelperT(VIEW* view, const QBrush& brush) :
	m_view {view},
	m_pen {QPen {}},
	m_brush {brush}
{}

template <typename VIEW>
ViewHelperT<VIEW>::ViewHelperT(VIEW* view, const QPen& pen, const QBrush& brush) :
	m_view {view},
	m_pen {pen},
	m_brush {brush}
{}

template <typename VIEW>
ViewHelperT<VIEW>::~ViewHelperT()
{}

template <typename VIEW>
QPen ViewHelperT<VIEW>::pen() const
{
	return m_pen;
}

template <typename VIEW>
void ViewHelperT<VIEW>::setPen(const QPen& pen)
{
	m_pen = pen;
}

template <typename VIEW>
QBrush ViewHelperT<VIEW>::brush() const
{
	return m_brush;
}

template <typename VIEW>
void ViewHelperT<VIEW>::setBrush(const QBrush& brush)
{
	m_brush = brush;
}

template <typename VIEW>
QPen& ViewHelperT<VIEW>::pen()
{
	return m_pen;
}

template <typename VIEW>
QBrush& ViewHelperT<VIEW>::brush()
{
	return m_brush;
}

template <typename VIEW>
bool ViewHelperT<VIEW>::preparePaintView(const QTransform&, const QRectF&)
{
	return true;
}

template <typename VIEW>
VIEW* ViewHelperT<VIEW>::view() const
{
	return m_view;
}

#endif // VIEWHELPERT_DETAIL_H
