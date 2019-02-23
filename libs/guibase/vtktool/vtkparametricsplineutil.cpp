#include "vtkparametricsplineutil.h"

#include <vtkCardinalSpline.h>
#include <vtkParametricSpline.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>

namespace {

const int SUBDIV_NUM = 3;

} // namespace

std::vector<QPointF> vtkParametricSplineUtil::buildSplinePointsWithSameDistances(vtkPoints* points, int divNum)
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

std::vector<QPointF> vtkParametricSplineUtil::buildSplinePoints(vtkPoints* points, int subDivNum)
{
	auto xSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto ySpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto zSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto spline = vtkSmartPointer<vtkParametricSpline>::New();

	spline->SetXSpline(xSpline);
	spline->SetYSpline(ySpline);
	spline->SetZSpline(zSpline);

	spline->SetPoints(points);

	int d = (points->GetNumberOfPoints() - 1) * subDivNum;

	std::vector<QPointF> ret;

	double u[3], v[3], Du[9];
	u[0] = 0; u[1] = 0; u[2] = 0;
	spline->Evaluate(u, v, Du);
	ret.push_back(QPointF(v[0], v[1]));
	for (int i = 1; i <= d; ++i) {
		u[0] = i / static_cast<double>(d);
		spline->Evaluate(u, v, Du);
		ret.push_back(QPointF(v[0], v[1]));
	}

	return ret;
}
