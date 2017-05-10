#ifndef LEFTBANKHWMPREPROCESSORVIEW_H
#define LEFTBANKHWMPREPROCESSORVIEW_H

#include "leftbankhwm.h"
#include "../points/pointspreprocessorview.h"

class LeftBankHWMPreProcessorView : public PointsPreProcessorView
{
public:
	LeftBankHWMPreProcessorView(Model* model, LeftBankHWM* item);
	~LeftBankHWMPreProcessorView();
};

#endif // LEFTBANKHWMPREPROCESSORVIEW_H
