#ifndef ARBITRARYHWMPREPROCESSORVIEW_H
#define ARBITRARYHWMPREPROCESSORVIEW_H

#include "arbitraryhwm.h"
#include "../points/pointspreprocessorview.h"

class ArbitraryHWMPreProcessorView : public PointsPreProcessorView
{
public:
	ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item);
	~ArbitraryHWMPreProcessorView();
};

#endif // ARBITRARYHWMPREPROCESSORVIEW_H
