#include "baseline.h"
#include "baselinepreprocessorcontroller.h"
#include "baselinepreprocessorview.h"
#include "../crosssection/crosssection.h"
#include "../../misc/geometryutil.h"

#include "private/baseline_impl.h"

#include <QStandardItem>
#include <QVector2D>

#include <algorithm>

BaseLine::BaseLine(DataItem *parent) :
	DataItem {parent},
	impl {new Impl {}}
{
	// for test
	// impl->m_polyLine.push_back(QPointF(0, 0));
	// impl->m_polyLine.push_back(QPointF(100, -20));
}

BaseLine::~BaseLine()
{
	delete impl;
}

const std::vector<QPointF>& BaseLine::polyLine() const
{
	return impl->m_polyLine;
}

void BaseLine::setPolyLine(const std::vector<QPointF>& line)
{
	impl->m_polyLine = line;
}

void BaseLine::getCrossingPoint(const CrossSection& cs, bool* crosses, double* x, double* y, double* pos)
{
	*crosses = false;
	*pos = 0;
	QPointF intersection;
	double r, s;
	for (int i = 0; i < impl->m_polyLine.size() - 1; ++i) {
		QPointF p1 = impl->m_polyLine.at(i);
		QPointF p2 = impl->m_polyLine.at(i + 1);
		bool localCross = GeometryUtil::intersectionPoint(
					p1, p2,
					cs.point1(), cs.point2(), &intersection, &r, &s);
		if (localCross) {
			if (r < 0 || r > 1) {localCross = false;}
			if (s < 0 || s > 1) {localCross = false;}
		}

		if (localCross) {
			// crosses.
			*x = intersection.x();
			*y = intersection.y();
			*pos = *pos + QVector2D(*x - p1.x(), *y - p1.y()).length();
			*crosses = true;
			return;
		} else {
			*pos = *pos + QVector2D(p2.x() - p1.x(), p2.y() - p1.y()).length();
		}
	}
}

void BaseLine::normalizeDirection(CrossSection* cs) const
{
	// @todo implement this
}

void BaseLine::reverseDirection()
{
	std::reverse(impl->m_polyLine.begin(), impl->m_polyLine.end());
}

QStandardItem* BaseLine::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Base Line"));
	setupStandardItem(item);
	return item;
}

DataItemController* BaseLine::buildPreProcessorDataItemController(Model* model)
{
	return new BaseLinePreProcessorController(model, this);
}

DataItemView* BaseLine::buildPreProcessorDataItemView(Model* model)
{
	return new BaseLinePreProcessorView(model, this);
}
