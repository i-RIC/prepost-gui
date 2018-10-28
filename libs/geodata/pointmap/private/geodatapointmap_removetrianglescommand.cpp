#include "geodatapointmap_removetrianglescommand.h"

GeoDataPointmap::RemoveTrianglesCommand::RemoveTrianglesCommand(vtkCellArray* newPolys, GeoDataPointmap* pointmap) :
	QUndoCommand(GeoDataPointmap::tr("Remove triangles with long edge")),
	m_newCellArray {newPolys},
	m_oldCellArray {pointmap->m_vtkDelaunayedPolyData->GetPolys()},
	m_pointmap {pointmap}
{}

void GeoDataPointmap::RemoveTrianglesCommand::redo()
{
	apply(m_newCellArray);
}

void GeoDataPointmap::RemoveTrianglesCommand::undo()
{
	apply(m_oldCellArray);
}

void GeoDataPointmap::RemoveTrianglesCommand::apply(vtkCellArray* cellArray)
{
	m_pointmap->m_vtkDelaunayedPolyData->SetPolys(cellArray);
	m_pointmap->m_vtkDelaunayedPolyData->Modified();
}
