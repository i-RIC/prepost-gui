#ifndef ARBITRARYHWMPREPROCESSORVIEW_H
#define ARBITRARYHWMPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class ArbitraryHWM;

class ArbitraryHWMPreProcessorView : public PointsPreProcessorView
{
public:
	ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item);
	~ArbitraryHWMPreProcessorView();
};

#endif // ARBITRARYHWMPREPROCESSORVIEW_H
