#ifndef RIGHTBANKHWMPREPROCESSORVIEWHELPER_H
#define RIGHTBANKHWMPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class RightBankHWMPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	RightBankHWMPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // RIGHTBANKHWMPREPROCESSORVIEWHELPER_H
