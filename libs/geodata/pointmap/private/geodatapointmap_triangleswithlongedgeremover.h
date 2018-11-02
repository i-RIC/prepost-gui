#ifndef GEODATAPOINTMAP_TRIANGLESWITHLONGEDGEREMOVER_H
#define GEODATAPOINTMAP_TRIANGLESWITHLONGEDGEREMOVER_H

#include "../geodatapointmap.h"

#include <guibase/vtktool/vtkpolydatapaintactor.h>

#include <set>
#include <vector>

class vtkCell;
class vtkPolyData;

class GeoDataPointmap::TrianglesWithLongEdgeRemover
{
public:
	TrianglesWithLongEdgeRemover(GeoDataPointmap* parent);
	~TrianglesWithLongEdgeRemover();

private:
	void buildEdgeLengthSqrts();
	double threshouldLength(double rate);

	void updatePreviewPolyData(double limitEdge, bool rescuePoints);
	void removeTriangles(double limitEdge, bool rescuePoints);

	vtkCellArray* buildPolyCellArray(double limitEdge, bool rescuePoints, bool forPreview);
	void rescuePoints(std::vector<vtkIdType>* cells);
	void rescueCellWithShortestEdges(const std::vector<vtkIdType>& cellsForPoint, std::vector<vtkIdType>* cells);

	vtkPolyDataPaintActor m_previewActor;
	vtkPolyData* m_previewPolyData;
	std::vector<double> m_edgeLengthSqrts;

	GeoDataPointmap* m_parent;

	class Dialog;
};

#endif // GEODATAPOINTMAP_TRIANGLESWITHLONGEDGEREMOVER_H
