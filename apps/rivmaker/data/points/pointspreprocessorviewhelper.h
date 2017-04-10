#ifndef POINTSPREPROCESSORVIEWHELPER_H
#define POINTSPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class QColor;

class PointsPreProcessorViewHelper : public DataItemViewHelperI
{
public:
	PointsPreProcessorViewHelper(DataItemView* v);

protected:
	void drawCircles(int size, const QColor& color, QPainter* painter) const;
	void drawRects(int size, const QColor& color, QPainter* painter) const;
};

#endif // POINTSPREPROCESSORVIEWHELPER_H
