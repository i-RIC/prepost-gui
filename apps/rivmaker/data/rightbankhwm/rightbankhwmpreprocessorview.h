#ifndef RIGHTBANKHWMPREPROCESSORVIEW_H
#define RIGHTBANKHWMPREPROCESSORVIEW_H

#include "rightbankhwm.h"
#include "../points/pointspreprocessorview.h"

class RightBankHWMPreProcessorView : public PointsPreProcessorView
{
public:
	RightBankHWMPreProcessorView(Model* model, RightBankHWM* item);
	~RightBankHWMPreProcessorView();
};

#endif // RIGHTBANKHWMPREPROCESSORVIEW_H
