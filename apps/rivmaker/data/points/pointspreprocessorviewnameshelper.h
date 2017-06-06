#ifndef POINTSPREPROCESSORVIEWNAMESHELPER_H
#define POINTSPREPROCESSORVIEWNAMESHELPER_H

#include "pointspreprocessorviewhelper.h"

class PointsPreProcessorViewNamesHelper : public PointsPreProcessorViewHelper
{
public:
	PointsPreProcessorViewNamesHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // POINTSPREPROCESSORVIEWNAMESHELPER_H
