#ifndef GEODATAPOINTMAP_TINMANAGER_TRIANGLESWITHLONGEDGEREMOVER_H
#define GEODATAPOINTMAP_TINMANAGER_TRIANGLESWITHLONGEDGEREMOVER_H

#include "geodatapointmap_tinmanager.h"

class GeoDataPointmap::TINManager::TrianglesWithLongEdgeRemover
{
public:
	static double thresholdLength(vtkPolyData* data, double rate);
	static vtkCellArray* buildCellArray(vtkPolyData* data, double limitEdge, bool rescuePoints, bool deleted);

private:
	static void rescuePoints(std::vector<vtkIdType>* cells, vtkPolyData* data);
	static void rescueCellWithShortestEdges(const std::vector<vtkIdType>& cellsForPoint, std::vector<vtkIdType>* cells, vtkPolyData* data);

	TrianglesWithLongEdgeRemover();
};

#endif // GEODATAPOINTMAP_TINMANAGER_TRIANGLESWITHLONGEDGEREMOVER_H
