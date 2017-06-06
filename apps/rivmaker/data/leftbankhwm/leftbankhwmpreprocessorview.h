#ifndef LEFTBANKHWMPREPROCESSORVIEW_H
#define LEFTBANKHWMPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class LeftBankHWM;

class LeftBankHWMPreProcessorView : public PointsPreProcessorView
{
public:
	LeftBankHWMPreProcessorView(Model* model, LeftBankHWM* item);
	~LeftBankHWMPreProcessorView();
};

#endif // LEFTBANKHWMPREPROCESSORVIEW_H
