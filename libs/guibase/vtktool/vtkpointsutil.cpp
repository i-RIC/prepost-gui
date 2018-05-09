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
