#ifndef REFERENCEMARKPREPROCESSORVIEWHELPER_H
#define REFERENCEMARKPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class ReferenceMarkPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	ReferenceMarkPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // REFERENCEMARKPREPROCESSORVIEWHELPER_H
