#include "geodatapointmap_tinmanager_removetrianglescommand.h"

GeoDataPointmap::TINManager::RemoveTrianglesCommand::RemoveTrianglesCommand(vtkCellArray* newPolys, TINManager* manager) :
	QUndoCommand(GeoDataPointmap::tr("Remove triangles with long edge")),
	m_newCellArray {newPolys},
	m_oldCellArray {manager->m_tin->GetPolys()},
	m_manager {manager}
{}

void GeoDataPointmap::TINManager::RemoveTrianglesCommand::redo()
{
	apply(m_newCellArray);
}

void GeoDataPointmap::TINManager::RemoveTrianglesCommand::undo()
{
	apply(m_oldCellArray);
}

void GeoDataPointmap::TINManager::RemoveTrianglesCommand::apply(vtkCellArray* cellArray)
{
	m_manager->m_tin->SetPolys(cellArray);
	m_manager->m_tin->Modified();
	m_manager->rebuildQTree();
}
