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

	QColor color = Qt::black;

	auto v = view();
	auto points = dynamic_cast<ElevationPoints*> (dataItem());

	const auto& pvec = points->points();

	painter->setBrush(color);
	painter->setPen(Qt::NoPen);
	for (QVector3D* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - pointSize * 0.5, p2.y() - pointSize * 0.5, pointSize, pointSize);
		painter->drawEllipse(rect);
	}

	painter->restore();
}
