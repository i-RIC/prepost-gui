#ifndef BENCHMARKPREPROCESSORVIEW_H
#define BENCHMARKPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class Benchmark;

class BenchmarkPreProcessorView : public PointsPreProcessorView
{
public:
	BenchmarkPreProcessorView(Model* model, Benchmark* item);
	~BenchmarkPreProcessorView();

private:
	void drawMarker(const QPointF& position, QPainter* painter) const override;
	void doDraw(QPainter *painter) const override;
};

#endif // BENCHMARKPREPROCESSORVIEW_H
