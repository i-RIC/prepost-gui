#ifndef VIEWHELPERT_H
#define VIEWHELPERT_H

#include <QBrush>
#include <QPen>

template <typename VIEW>
class ViewHelperT
{
public:
	ViewHelperT(VIEW* modelview);
	ViewHelperT(VIEW* modelview, const QPen& pen);
	ViewHelperT(VIEW* modelview, const QBrush& brush);
	ViewHelperT(VIEW* modelview, const QPen& pen, const QBrush& brush);
	virtual ~ViewHelperT();

	QPen pen() const;
	void setPen(const QPen& pen);

	QBrush brush() const;
	void setBrush(const QBrush& brush);

	virtual bool preparePaintView(const QTransform& transform, const QRectF& rect);
	virtual void paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const = 0;

protected:
	VIEW* view() const;
	QPen& pen();
	QBrush& brush();

private:
	VIEW* m_view;
	QPen m_pen;
	QBrush m_brush;
};

#include "viewhelpert_detail.h"

#endif // VIEWHELPERT_H
