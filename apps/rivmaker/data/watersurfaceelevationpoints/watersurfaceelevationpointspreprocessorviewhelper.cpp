#include "watersurfaceelevationpoints.h"
#include "watersurfaceelevationpointspreprocessorviewhelper.h"

#include "../base/model.h"
#include "../base/view.h"

#include <QPainter>
#include <QVector3D>

namespace {

int pointSize = 5;

void drawPoints(const std::vector<QVector3D*>& points, const QColor& color, View* v, QPainter* painter)
{
	for (QVector3D* p : points) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));
		QRectF rect(p2.x() - pointSize * 0.5, p2.y() - pointSize * 0.5, pointSize, pointSize);
		painter->fillRect(rect, color);
	}
}

} // namespace

WaterSurfaceElevationPointsPreProcessorViewHelper::WaterSurfaceElevationPointsPreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void WaterSurfaceElevationPointsPreProcessorViewHelper::draw(QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<WaterSurfaceElevationPoints*> (dataItem());

	drawPoints(points->leftBankPoints(), Qt::blue, v, painter);
	drawPoints(points->rightBankPoints(), Qt::red, v, painter);
	drawPoints(points->arbitraryPoints(), Qt::darkGray, v, painter);

	painter->restore();
}
