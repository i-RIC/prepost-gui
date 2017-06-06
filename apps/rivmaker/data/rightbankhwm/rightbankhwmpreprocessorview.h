#ifndef RIGHTBANKHWMPREPROCESSORVIEW_H
#define RIGHTBANKHWMPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class RightBankHWM;

class RightBankHWMPreProcessorView : public PointsPreProcessorView
{
public:
	RightBankHWMPreProcessorView(Model* model, RightBankHWM* item);
	~RightBankHWMPreProcessorView();
};

#endif // RIGHTBANKHWMPREPROCESSORVIEW_H
