#include "vtkpointsutil.h"

#include <QPointF>
#include <QPolygonF>

#include <vtkCell.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPointSet.h>
#include <vtkSmartPointer.h>

#include <set>

namespace {

bool edgeContainsPoint(vtkCell* edgeCell, vtkIdType vid)
{
	for (vtkIdType i = 0; i < edgeCell->GetNumberOfPoints(); ++i) {
		vtkIdType p = edgeCell->GetPointId(i);
		if (p == vid) {return true;}
	}
	return false;
}

} // namespace

std::vector<QPointF> vtkPointsUtil::getCoordinates(vtkPoints* points)
{
	std::vector<QPointF> coords;
	double p[3];
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, p);
		coords.push_back(QPointF(p[0], p[1]));
	}
	return coords;
}

std::vector<QPointF> vtkPointsUtil::getCoordinates(vtkPointSet* pointset)
{
	return getCoordinates(pointset->GetPoints());
}

std::vector<QPointF> vtkPointsUtil::getCoordinates(vtkCell* cell)
{
	return getCoordinates(cell->GetPoints());
}

QPointF vtkPointsUtil::getCenter(vtkCell* cell)
{
	double v[3], p[3];
	for (int i = 0; i < 3; ++i) {
		v[i] = 0;
	}

	auto points = cell->GetPoints();
	int num_p = cell->GetNumberOfPoints();
	for (int i = 0; i < num_p; ++i) {
		points->GetPoint(i, p);
		for (int j = 0; j < 3; ++j) {
			v[j] += p[j];
		}
	}
	return QPointF(v[0] / num_p, v[1] / num_p);
}

void vtkPointsUtil::getBoundingBox(vtkCell* cell, double* xmin, double* xmax, double* ymin, double* ymax)
{
	auto points = cell->GetPoints();
	int num_p = cell->GetNumberOfPoints();
	double p[3];
	for (int i = 0; i < num_p; ++i) {
		points->GetPoint(i, p);
		if (i == 0 || p[0] < *xmin) {*xmin = p[0];}
		if (i == 0 || p[0] > *xmax) {*xmax = p[0];}
		if (i == 0 || p[1] < *ymin) {*ymin = p[1];}
		if (i == 0 || p[1] > *ymax) {*ymax = p[1];}
	}
}

void vtkPointsUtil::getBoundingBox(const std::vector<QPointF>& points, double* xmin, double* xmax, double* ymin, double* ymax)
{
	for (int i = 0; i < points.size(); ++i) {
		const QPointF& p = points.at(i);
		if (i == 0 || p.x() < *xmin) {*xmin = p.x();}
		if (i == 0 || p.x() > *xmax) {*xmax = p.x();}
		if (i == 0 || p.y() < *ymin) {*ymin = p.y();}
		if (i == 0 || p.y() > *ymax) {*ymax = p.y();}
	}
}

QPolygonF vtkPointsUtil::getPolygon(vtkCell* cell)
{
	auto points = cell->GetPoints();
	int num_p = cell->GetNumberOfPoints();
	double p[3];

	QPolygonF ret;

	for (int i = 0; i < num_p; ++i) {
		points->GetPoint(i, p);
		ret.append(QPointF(p[0], p[1]));
	}
	points->GetPoint(0, p);
	ret.append(QPointF(p[0], p[1]));

	return ret;
}

std::vector<QPointF> vtkPointsUtil::getEdgeOtherPoints(vtkPointSet* pointset, vtkIdType vid)
{
	auto cellIds = vtkSmartPointer<vtkIdList>::New();
	pointset->GetPointCells(vid, cellIds);

	std::set<vtkIdType> otherPoints;
	for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i) {
		vtkIdType cellId = cellIds->GetId(i);
		vtkCell* cell = pointset->GetCell(cellId);
		for (vtkIdType j = 0; j < cell->GetNumberOfEdges(); ++j) {
			auto edge = cell->GetEdge(j);
			if (! edgeContainsPoint(edge, vid)) {continue;}

			for (vtkIdType k = 0; k < edge->GetNumberOfPoints(); ++k) {
				vtkIdType id = edge->GetPointId(k);
				if (id == vid) {continue;}
				otherPoints.insert(id);
			}
		}
	}
	std::vector<QPointF> ret;
	double p[3];
	for (vtkIdType vid : otherPoints) {
		pointset->GetPoint(vid, p);
		ret.push_back(QPointF(p[0], p[1]));
	}
	return ret;
}

void vtkPointsUtil::setCoordinates(vtkPoints* points, const std::vector<QPointF>& coords)
{
	points->SetNumberOfPoints(coords.size());
	for (vtkIdType i = 0; i < coords.size(); ++i) {
		auto p = coords.at(i);
		points->SetPoint(i, p.x(), p.y(), 0);
	}
	points->Modified();
}

void vtkPointsUtil::setCoordinates(vtkPointSet* pointset, const std::vector<QPointF>& coords)
{
	setCoordinates(pointset->GetPoints(), coords);
	pointset->Modified();
}

void vtkPointsUtil::setCoordinates(vtkCell* cell, const std::vector<QPointF>& coords)
{
	setCoordinates(cell->GetPoints(), coords);
	cell->Modified();
}
