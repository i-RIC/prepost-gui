#ifndef STREAMGAGEPREPROCESSORVIEWHELPER_H
#define STREAMGAGEPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class StreamGagePreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	StreamGagePreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // STREAMGAGEPREPROCESSORVIEWHELPER_H
