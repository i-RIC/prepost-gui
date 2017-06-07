#include "benchmark.h"
#include "benchmarkpreprocessorview.h"
#include "benchmarkpreprocessorviewhelper.h"
#include "../base/view.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

BenchmarkPreProcessorView::BenchmarkPreProcessorView(Model* model, Benchmark* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new BenchmarkPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

BenchmarkPreProcessorView::~BenchmarkPreProcessorView()
{}

void BenchmarkPreProcessorView::drawMarker(const QPointF& position, QPainter* painter) const
{
	PointsPreProcessorViewHelper::drawReverseTriangle(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::black, 0, painter);
}

void BenchmarkPreProcessorView::doDraw(QPainter *painter) const
{
	DataItemView::doDraw(painter);
	QSize size = view()->size();

	drawLegend(QPointF(size.width() - LEGEND_WIDTH, LEGEND_TOPMARGIN + LEGEND_LINEHEIGHT), painter);
}
