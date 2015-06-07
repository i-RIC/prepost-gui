#include "gridcreatingconditioncompoundchannelspline.h"

#include <QLineF>
#include <QPointF>
#include <QVector2D>
#include <QVector>

#include <vtkPoints.h>

#define SUBDIV 5
#define ITERATION 10

const QPointF GridCreatingConditionCompoundChannelSpline::errorPoint(GridCreatingConditionCompoundChannelSpline::ERROR_COORD, GridCreatingConditionCompoundChannelSpline::ERROR_COORD);

GridCreatingConditionCompoundChannelSpline::GridCreatingConditionCompoundChannelSpline()
{
	m_xSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_ySpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_zSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_spline = vtkSmartPointer<vtkParametricSpline>::New();

	m_spline->SetXSpline(m_xSpline);
	m_spline->SetYSpline(m_ySpline);
	m_spline->SetZSpline(m_zSpline);
}

GridCreatingConditionCompoundChannelSpline::~GridCreatingConditionCompoundChannelSpline()
{

}

void GridCreatingConditionCompoundChannelSpline::setPoints(QVector<QPointF> list)
{
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	points->Allocate(list.count());
	for (int i = 0; i < list.count(); ++i) {
		QPointF p = list.at(i);
		points->InsertNextPoint(p.x(), p.y(), 0);
	}
	m_spline->SetPoints(points);
	m_spline->Modified();
}

void GridCreatingConditionCompoundChannelSpline::setupPoints(int count)
{
	m_splinePoints.clear();
	double u[3], pt[3], du[9];
	for (int i = 0; i < count; ++i) {
		u[0] = i / static_cast<double>(count - 1);
		m_spline->Evaluate(u, pt, du);
		m_splinePointParameters.append(u[0]);
		m_splinePoints.append(QVector2D(pt[0], pt[1]));
	}
}

QVector<QPointF> GridCreatingConditionCompoundChannelSpline::getEquallySpacedPoints(int division) const
{
	QVector<QVector2D> tmpPoints;
	QVector<double> tmpDistances;
	int subCount = division * SUBDIV;
	double distance = 0;
	QVector2D oldP;
	double u[3], pt[3], du[9];
	for (int i = 0; i < subCount; ++i) {
		u[0] = i / static_cast<double>(subCount - 1);
		m_spline->Evaluate(u, pt, du);
		QVector2D newP(pt[0], pt[1]);
		if (i == 0) {
			distance = 0;
		} else {
			distance += (newP - oldP).length();
		}
		tmpPoints.append(newP);
		tmpDistances.append(distance);
		oldP = newP;
	}
	// tmpPoints, tmpDistances setup.

	QVector<QPointF> ret;

	// add the first point.
	u[0] = 0;
	m_spline->Evaluate(u, pt, du);
	ret.append(QPointF(pt[0], pt[1]));
	int addedPoints = 1;
	double deltaDist = distance / division;
	double nextDistance = deltaDist;
	for (int i = 0; i < subCount && addedPoints < division; ++i) {
		if (tmpDistances[i] > nextDistance) {
			u[0] = (i - 1 +
							(nextDistance - tmpDistances[i - 1]) / (tmpDistances[i] - tmpDistances[i - 1])
						 ) / static_cast<double>(subCount - 1);
			m_spline->Evaluate(u, pt, du);
			ret.append(QPointF(pt[0], pt[1]));
			++ addedPoints;
			nextDistance += deltaDist;
		}
	}
	// add the last point;
	u[0] = 1;
	m_spline->Evaluate(u, pt, du);
	ret.append(QPointF(pt[0], pt[1]));

	return ret;
}

QPointF GridCreatingConditionCompoundChannelSpline::getNearestPoint(const QPointF& p) const
{
	QVector<double> distances;
	int minIndex = 0;
	double minDistance;
	for (int i = 0; i < m_splinePoints.count(); ++i) {
		QVector2D tmpV = m_splinePoints.at(i);
		double distance = (tmpV - QVector2D(p.x(), p.y())).lengthSquared();
		distances.append(distance);
		if (i == 0 || distance < minDistance) {
			minIndex = i;
			minDistance = distance;
		}
	}
	int centerIndex;
	if (minIndex == 0) {
		centerIndex = 1;
	} else if (minIndex == m_splinePoints.size() - 1) {
		centerIndex = m_splinePoints.size() - 2;
	} else {
		centerIndex = minIndex;
	}
	return twoDiv(m_splinePointParameters[centerIndex - 1], m_splinePointParameters[centerIndex], m_splinePointParameters[centerIndex + 1], distances[centerIndex - 1], distances[centerIndex], distances[centerIndex + 1], p, ITERATION);
}

QPointF GridCreatingConditionCompoundChannelSpline::twoDiv(double leftP, double centerP, double rightP, double leftV, double centerV, double rightV, const QPointF& p, int depthLeft) const
{
	double u[3], pt[3], du[9];
	if (depthLeft == 0) {
		if (leftV < centerV && leftV < rightV) {
			u[0] = leftP;
		} else if (rightV < centerV && rightV < leftV) {
			u[0] = rightP;
		} else {
			u[0] = centerP;
		}
		m_spline->Evaluate(u, pt, du);
		return QPointF(pt[0], pt[1]);
	}
	double leftCenterP = (leftP + centerP) * 0.5;
	u[0] = leftCenterP;
	m_spline->Evaluate(u, pt, du);
	double leftCenterV = (QVector2D(pt[0], pt[1]) - QVector2D(p.x(), p.y())).lengthSquared();

	double rightCenterP = (rightP + centerP) * 0.5;
	u[0] = rightCenterP;
	m_spline->Evaluate(u, pt, du);
	double rightCenterV = (QVector2D(pt[0], pt[1]) - QVector2D(p.x(), p.y())).lengthSquared();

	if (leftCenterV < centerV && leftCenterV < rightCenterV) {
		return twoDiv(leftP, leftCenterP, centerP, leftV, leftCenterV, centerV, p, depthLeft - 1);
	} else if (rightCenterV < centerV && rightCenterV < leftCenterV) {
		return twoDiv(centerP, rightCenterP, rightP, centerV, rightCenterV, rightV, p, depthLeft - 1);
	} else {
		return twoDiv(leftCenterP, centerP, rightCenterP, leftCenterV, centerV, rightCenterV, p, depthLeft - 1);
	}
}

QPointF GridCreatingConditionCompoundChannelSpline::getIntersectPoint(const QLineF& f) const
{
	bool ok = false;
	QPointF okp;
	int minIndex = -1;
	double minDistanceSqr;
	/*
		QFile file("log.txt");
		QFile file2("log.vtk");
		file.open(QFile::Append);
		file2.open(QFile::WriteOnly);
		QTextStream s(&file);
		QTextStream s2(&file2);
		s << "Intersect\n";
		s << "Line: "
				<< "(" << f.p1().x() << "," << f.p1().y() << ") - "
				<< "(" << f.p2().x() << "," << f.p2().y() << ") \n";
		s2
				<< "# vtk DataFile Version 3.0\r\n"
				<< "vtk output\r\n"
				<< "ASCII\r\n"
				<< "DATASET UNSTRUCTURED_GRID\r\n"
				<< "POINTS " << m_splinePoints.count() * 2 << " double\r\n";
		s2 << f.p1().x() << " " << f.p1().y() << " 0\r\n";
		s2 << f.p2().x() << " " << f.p2().y() << " 0\r\n";
	*/
	for (int i = 0; i < m_splinePoints.count() - 1; ++i) {
		QVector2D v1 = m_splinePoints.at(i);
		QVector2D v2 = m_splinePoints.at(i + 1);
		QLineF edge(QPointF(v1.x(), v1.y()), QPointF(v2.x(), v2.y()));
		/*
				s << "Target: "
						<< "(" << edge.p1().x() << "," << edge.p1().y() << ") - "
						<< "(" << edge.p2().x() << "," << edge.p2().y() << ") \n";
				s2 << edge.p1().x() << " " << edge.p1().y() << " 0\r\n";
				s2 << edge.p2().x() << " " << edge.p2().y() << " 0\r\n";
		*/
		QPointF p;
		QLineF::IntersectType iType = edge.intersect(f, &p);
		if (iType != QLineF::NoIntersection) {
			QLineF newF(f.p1(), p + f.p2() - f.p1());
			/*
						s << "HIT!\n";
						s << "Target2: "
								<< "(" << newF.p1().x() << "," << newF.p1().y() << ") - "
								<< "(" << newF.p2().x() << "," << newF.p2().y() << ") \n";
			*/
			if (QLineF::BoundedIntersection == edge.intersect(newF, &p)) {
				/*
								s << "INTERSECT!\n";
				*/
				QPointF diff = f.p1() - p;
				QVector2D diffV(diff.x(), diff.y());
				double distanceSqr = diffV.lengthSquared();
				if (minIndex == -1 || distanceSqr < minDistanceSqr) {
					okp = p;
					minIndex = i;
					minDistanceSqr = distanceSqr;
					ok = true;
				}
			}
		}
	}
	/*
		s2 << "CELLS " << m_splinePoints.count() << " " << 3 * m_splinePoints.count() << "\r\n";
		for (int i = 0; i < m_splinePoints.count(); ++i){
			s2 << "2 " << i * 2 << " " << i * 2 + 1 << "\r\n";
		}
		s2 << "CELL_TYPES " << m_splinePoints.count()<< "\r\n";
		for (int i = 0; i < m_splinePoints.count(); ++i){
			s2 << "3\r\n";
		}
		s << "FAIL!\n";
		file2.close();
	*/
	if (ok) {
		return okp;
	} else {
		return errorPoint;
	}
}
