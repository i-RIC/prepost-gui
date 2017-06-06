#ifndef POINTSPREPROCESSORVIEWHELPER_H
#define POINTSPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class QColor;

class PointsPreProcessorViewHelper : public DataItemViewHelperI
{
public:
	PointsPreProcessorViewHelper(DataItemView* v);

protected:
	void drawCircles(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawRects(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawDiamonds(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawTriangles(int size, const QColor& color, int transparency, QPainter* painter) const;
	void drawReverseTriangles(int size, const QColor& color, int transparency, QPainter* painter) const;

	const static int STD_SIZE;
};

#endif // POINTSPREPROCESSORVIEWHELPER_H
