#ifndef HUBPREPROCESSORVIEW_H
#define HUBPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class Hub;

class HubPreProcessorView : public PointsPreProcessorView
{
public:
	HubPreProcessorView(Model* model, Hub* item);
	~HubPreProcessorView();
};

#endif // HUBPREPROCESSORVIEW_H
