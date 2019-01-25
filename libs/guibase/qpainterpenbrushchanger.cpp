#include "qpainterpenbrushchanger.h"

#include <QPainter>

QPainterPenBrushChanger::QPainterPenBrushChanger(QPainter* painter, const QPen& pen, const QBrush& brush) :
	m_restorer{painter}
{
	painter->setPen(pen);
	painter->setBrush(brush);
}

QPainterPenBrushChanger::~QPainterPenBrushChanger()
{}
