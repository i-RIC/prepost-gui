#ifndef HUBPREPROCESSORVIEWHELPER_H
#define HUBPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class HubPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	HubPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // HUBPREPROCESSORVIEWHELPER_H
