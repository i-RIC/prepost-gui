#include "baseline.h"
#include "baselinepreprocessorcontroller.h"
#include "baselinepreprocessorview.h"
#include "../crosssection/crosssection.h"
#include "../../misc/geometryutil.h"

#include "private/baseline_impl.h"

#include <QFile>
#include <QIcon>
#include <QPointF>
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

std::vector<QPointF> BaseLine::coordinates() const
{
	return polyLine();
}

void BaseLine::setCoordinates(const std::vector<QPointF>& coords)
{
	setPolyLine(coords);
}

void BaseLine::getCrossingPoint(CrossSection* cs, bool* crosses, double* x, double* y, double* pos) const
{
	if (impl->m_polyLine.size() < 2) {return;}

	*crosses = false;
	*pos = 0;
	QPointF intersection;
	double r, s;
	for (int i = 0; i < impl->m_polyLine.size() - 1; ++i) {
		QPointF p1 = impl->m_polyLine.at(i);
		QPointF p2 = impl->m_polyLine.at(i + 1);
		bool localCross = GeometryUtil::intersectionPoint(
					p1, p2,
					cs->point1(), cs->point2(), &intersection, &r, &s);
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

			// reverse direction of crosssection if needed.
			QPointF v1 = p2 - p1;
			QPointF v2 = cs->point2() - cs->point1();
			double outerProd = v1.x() * v2.y() - v1.y() * v2.x();
			if (outerProd < 0) {
				cs->reverseDirection();
			}
			return;
		}
		else {
			*pos = *pos + QVector2D(p2.x() - p1.x(), p2.y() - p1.y()).length();
		}
	}
}

double BaseLine::calcPosition(double x, double y) const
{
	if (impl->m_polyLine.size() < 2) {return 0;}

	std::map<double, double> distanceMap;
	double pos = 0;
	for (int i = 0; i < impl->m_polyLine.size() - 1; ++i) {
		QPointF p1 = impl->m_polyLine.at(i);
		QPointF p2 = impl->m_polyLine.at(i + 1);

		QPointF np = GeometryUtil::nearestPoint(p1, p2, QPointF(x, y));
		double point_pos = pos + QVector2D(np.x() - p1.x(), np.y() - p1.y()).length();
		double distSqr = QVector2D(np.x() - x, np.y() - y).lengthSquared();
		distanceMap.insert(std::make_pair(distSqr, point_pos));

		pos += QVector2D(p2.x() - p1.x(), p2.y() - p1.y()).length();
	}
	auto it = distanceMap.begin();
	return it->second;
}

void BaseLine::reverseDirection()
{
	std::reverse(impl->m_polyLine.begin(), impl->m_polyLine.end());
}

QStandardItem* BaseLine::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconBaseline.png"),tr("Base Line"));
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

void BaseLine::loadExternalData(const QString& filename)
{
	QFile file(filename);
	bool ok = file.open(QIODevice::ReadOnly);
	if (! ok) {return;}

	QDataStream in(&file);
	in.setVersion(QDATASTREAM_VERSION);

	qint32 count;

	in >> count;

	impl->m_polyLine.clear();
	for (qint32 i = 0; i < count; ++i) {
		QPointF p;
		in >> p;
		impl->m_polyLine.push_back(p);
	}

	file.close();
}

void BaseLine::saveExternalData(const QString& filename) const
{
	QFile file(filename);
	bool ok = file.open(QIODevice::WriteOnly);
	if (! ok) {return;}
	QDataStream out(&file);
	out.setVersion(QDATASTREAM_VERSION);

	qint32 count = static_cast<qint32> (impl->m_polyLine.size());

	out << count;

	for (QPointF& p : impl->m_polyLine) {
		out << p;
	}

	file.close();
}

QString BaseLine::relativeFilename() const
{
	return "baseline.dat";
}
