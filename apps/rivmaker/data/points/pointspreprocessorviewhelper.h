#ifndef POINTSPREPROCESSORVIEWHELPER_H
#define POINTSPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class QColor;
class QPointF;

class PointsPreProcessorViewHelper : public DataItemViewHelperI
{
public:
	const static int STD_SIZE;

	PointsPreProcessorViewHelper(DataItemView* v);

	static void drawCircle(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter);
	static void drawRect(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter);
	static void drawDiamond(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter);
	static void drawTriangle(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter);
	static void drawReverseTriangle(const QPointF& position, int size, const QColor& color, int transparency, QPainter* painter);

protected:
	void drawCircles(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawRects(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawDiamonds(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawTriangles(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawReverseTriangles(int size, const QColor& color, int transparency, QPainter* painter) const;

	void drawNames(QPainter* painter) const;
};

#endif // POINTSPREPROCESSORVIEWHELPER_H
