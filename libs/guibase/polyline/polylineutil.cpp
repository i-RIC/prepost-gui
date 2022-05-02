#include "polylineutil.h"

#include <vtkCardinalSpline.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <QPointF>
#include <QRectF>

namespace {

const int SUBDIV_NUM = 3;

} // namespace

std::vector<QPointF> PolyLineUtil::buildSplinePoints(vtkPoints* points, int divNum)
{
	auto xSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto ySpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto zSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto spline = vtkSmartPointer<vtkParametricSpline>::New();

	spline->SetXSpline(xSpline);
	spline->SetYSpline(ySpline);
	spline->SetZSpline(zSpline);

	spline->SetPoints(points);

	int d = divNum * SUBDIV_NUM;

	std::vector<double> length(d + 1);
	length[0] = 0;
	double u[3], v_prev[3], v[3], Du[9];
	u[0] = 0; u[1] = 0; u[2] = 0;
	spline->Evaluate(u, v_prev, Du);
	for (int i = 1; i <= d; ++i) {
		u[0] = i / static_cast<double>(d);
		spline->Evaluate(u, v, Du);
		double dx = v[0] - v_prev[0];
		double dy = v[1] - v_prev[1];
		length[i] = length[i - 1] + sqrt(dx * dx + dy * dy);

		for (int j = 0; j < 3; ++j) {
			v_prev[j] = v[j];
		}
	}
	double wholeLen = length.at(length.size() - 1);

	int idx = 0;
	std::vector<QPointF> ret;

	for (int i = 0; i <= divNum; ++i) {
		double param = 0;
		if (i != 0) {
			double r = i / static_cast<double>(divNum);
			if (r > 1) {r = 1;}
			double targetLen = wholeLen * r;
			while (length[idx + 1] < targetLen) {
				++ idx;
			}
			double l1 = length[idx];
			double l2 = length[idx + 1];
			double target_idx = idx + 1.0 / (l2 - l1) * (targetLen - l1);
			param = target_idx / d;
		}
		u[0] = param;
		spline->Evaluate(u, v, Du);

		ret.push_back(QPointF(v[0], v[1]));
	}
	return ret;
}

double PolyLineUtil::length(const std::vector<QPointF>& polyLine)
{
	double len = 0;

	QPointF prev_p = polyLine.at(0);
	for (QPointF p : polyLine) {
		double dx = p.x() - prev_p.x();
		double dy = p.y() - prev_p.y();

		len = len + sqrt(dx * dx + dy * dy);
		prev_p = p;
	}
	return len;
}

QRectF PolyLineUtil::boundingRect(const std::vector<QPointF>& polyLine)
{
	double xmin, xmax, ymin, ymax;
	for (int i = 0; i < polyLine.size(); ++i) {
		const auto& p = polyLine.at(i);
		double x = p.x();
		double y = p.y();
		if (i == 0 || x < xmin) {xmin = x;}
		if (i == 0 || x > xmax) {xmax = x;}
		if (i == 0 || y < ymin) {ymin = y;}
		if (i == 0 || y > ymax) {ymax = y;}
	}
	return QRectF(xmin, ymin, xmax - xmin, ymax - ymin);
}

PolyLineUtil::PolyLineUtil()
{}
