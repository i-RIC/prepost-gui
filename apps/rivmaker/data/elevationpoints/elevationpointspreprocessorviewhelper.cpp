#include "elevationpoints.h"
#include "elevationpointspreprocessorviewhelper.h"

#include "../base/model.h"
#include "../base/view.h"

#include <QPainter>
#include <QVector3D>

namespace {

int pointSize = 5;

} // namespace

ElevationPointsPreProcessorViewHelper::ElevationPointsPreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void ElevationPointsPreProcessorViewHelper::draw(QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<ElevationPoints*> (dataItem());

	const auto& pvec = points->points();
	for (QVector3D* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));
		QRectF rect(p2.x() - pointSize * 0.5, p2.y() - pointSize * 0.5, pointSize, pointSize);

		painter->fillRect(rect, Qt::darkGreen);
	}

	painter->restore();
}
