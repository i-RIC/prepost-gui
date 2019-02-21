#ifndef QPAINTERPENBRUSHRESTORER_H
#define QPAINTERPENBRUSHRESTORER_H

#include "guibase_global.h"

#include <QBrush>
#include <QPen>

class QPainter;

class GUIBASEDLL_EXPORT QPainterPenBrushRestorer
{
public:
	QPainterPenBrushRestorer(QPainter* painter);
	~QPainterPenBrushRestorer();

private:
	QPainter* m_painter;

	QBrush m_oldBrush;
	QPen m_oldPen;
};

#endif // QPAINTERPENBRUSHRESTORER_H
