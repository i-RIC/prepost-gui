#ifndef QPAINTERPENBRUSHCHANGER_H
#define QPAINTERPENBRUSHCHANGER_H

#include "guibase_global.h"

#include "qpainterpenbrushrestorer.h"

class QBrush;
class QPainter;
class QPen;

class GUIBASEDLL_EXPORT QPainterPenBrushChanger
{
public:
	QPainterPenBrushChanger(QPainter* painter, const QPen& pen, const QBrush& brush);
	~QPainterPenBrushChanger();

private:
	QPainterPenBrushRestorer m_restorer;
};

#endif // QPAINTERPENBRUSHCHANGER_H
