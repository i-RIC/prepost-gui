#ifndef BENCHMARKPREPROCESSORVIEWHELPER_H
#define BENCHMARKPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class BenchmarkPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	BenchmarkPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // BENCHMARKPREPROCESSORVIEWHELPER_H
