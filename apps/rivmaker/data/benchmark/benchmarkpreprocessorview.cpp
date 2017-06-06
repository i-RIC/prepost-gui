#include "benchmark.h"
#include "benchmarkpreprocessorview.h"
#include "benchmarkpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

BenchmarkPreProcessorView::BenchmarkPreProcessorView(Model* model, Benchmark* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new BenchmarkPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

BenchmarkPreProcessorView::~BenchmarkPreProcessorView()
{}
