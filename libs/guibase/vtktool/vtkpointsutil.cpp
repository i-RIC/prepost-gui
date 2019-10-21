#include "vtkpointsutil.h"

#include <QPointF>

#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkPointSet.h>

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
