#ifndef ARBITRARYHWMPREPROCESSORVIEWHELPER_H
#define ARBITRARYHWMPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class ArbitraryHWMPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	ArbitraryHWMPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // ARBITRARYHWMPREPROCESSORVIEWHELPER_H
