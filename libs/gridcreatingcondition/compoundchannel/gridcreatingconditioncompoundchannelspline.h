#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNELSPLINE_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNELSPLINE_H

#include <vtkSmartPointer.h>
#include <vtkCardinalSpline.h>
#include <vtkParametricSpline.h>
#include <QVector>

class QPointF;
class QLineF;
class QVector2D;

class GridCreatingConditionCompoundChannelSpline
{
	const static int ERROR_COORD = -1E8;
public:
	GridCreatingConditionCompoundChannelSpline();
	~GridCreatingConditionCompoundChannelSpline();
	void setPoints(QVector<QPointF>);
	void setupPoints(int count);

	QVector<QPointF> getEquallySpacedPoints(int division) const;
	QPointF getNearestPoint(const QPointF& p) const;
	QPointF getIntersectPoint(const QLineF& f) const;
	const static QPointF errorPoint;
private:
	QPointF twoDiv(double leftP, double centerP, double rightP, double leftV, double centerV, double rightV, const QPointF& p, int depthLeft) const;
	vtkSmartPointer<vtkParametricSpline> m_spline;
	vtkSmartPointer<vtkCardinalSpline> m_xSpline;
	vtkSmartPointer<vtkCardinalSpline> m_ySpline;
	vtkSmartPointer<vtkCardinalSpline> m_zSpline;

	QVector<QVector2D> m_splinePoints;
	QVector<double> m_splinePointParameters;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNELSPLINE_H
