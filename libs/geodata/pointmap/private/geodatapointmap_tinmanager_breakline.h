#ifndef GEODATAPOINTMAP_TINMANAGER_BREAKLINE_H
#define GEODATAPOINTMAP_TINMANAGER_BREAKLINE_H

#include "geodatapointmap_tinmanager.h"
#include <guibase/polyline/polylinecontroller.h>

class vtkPolyData;

class GeoDataPointmap::TINManager::BreakLine
{
public:
	BreakLine(vtkPolyData* points);

	const std::vector<vtkIdType>& vertexIndices() const;
	void setVertexIndices(const std::vector<vtkIdType>& indices);
	void cleanVertexIndices();

	bool intersects(const BreakLine& line) const;

	const PolyLineController& controller() const;

	void addActorsToRenderer(vtkRenderer* renderer);
	void removeActorsFromRenderer(vtkRenderer* renderer);

private:
	void updateShapeData();

	PolyLineController m_controller;
	std::vector<vtkIdType> m_vertexIndices;
	vtkPolyData* m_points;
};

#endif // GEODATAPOINTMAP_TINMANAGER_BREAKLINE_H
