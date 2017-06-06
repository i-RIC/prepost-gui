#ifndef REFERENCEMARKPREPROCESSORVIEW_H
#define REFERENCEMARKPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class ReferenceMark;

class ReferenceMarkPreProcessorView : public PointsPreProcessorView
{
public:
	ReferenceMarkPreProcessorView(Model* model, ReferenceMark* item);
	~ReferenceMarkPreProcessorView();
};

#endif // REFERENCEMARKPREPROCESSORVIEW_H
