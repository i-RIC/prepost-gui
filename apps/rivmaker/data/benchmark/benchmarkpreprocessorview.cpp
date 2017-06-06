#include "benchmark.h"
#include "benchmarkpreprocessorview.h"
#include "benchmarkpreprocessorviewhelper.h"

BenchmarkPreProcessorView::BenchmarkPreProcessorView(Model* model, Benchmark* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new BenchmarkPreProcessorViewHelper(this));
}

BenchmarkPreProcessorView::~BenchmarkPreProcessorView()
{}
