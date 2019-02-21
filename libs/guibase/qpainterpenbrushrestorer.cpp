#include "qpainterpenbrushrestorer.h"

#include <QPainter>

QPainterPenBrushRestorer::QPainterPenBrushRestorer(QPainter* painter) :
	m_painter {painter},
	m_oldBrush {painter->brush()},
	m_oldPen {painter->pen()}
{}

QPainterPenBrushRestorer::~QPainterPenBrushRestorer()
{
	m_painter->setBrush(m_oldBrush);
	m_painter->setPen(m_oldPen);
}
