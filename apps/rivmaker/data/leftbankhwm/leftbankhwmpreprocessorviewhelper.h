#ifndef LEFTBANKHWMPREPROCESSORVIEWHELPER_H
#define LEFTBANKHWMPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class LeftBankHWMPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	LeftBankHWMPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // LEFTBANKHWMPREPROCESSORVIEWHELPER_H
