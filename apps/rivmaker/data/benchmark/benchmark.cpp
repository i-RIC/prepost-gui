#include "benchmark.h"
#include "benchmarkpreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

Benchmark::Benchmark(DataItem* parent) :
	Points {parent}
{}

Benchmark::~Benchmark()
{}

QStandardItem* Benchmark::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconBenchmark.png"), tr("Benchmark"));
	setupStandardItem(item);
	return item;
}

DataItemView* Benchmark::buildPreProcessorDataItemView(Model* model)
{
	return new BenchmarkPreProcessorView(model, this);
}
