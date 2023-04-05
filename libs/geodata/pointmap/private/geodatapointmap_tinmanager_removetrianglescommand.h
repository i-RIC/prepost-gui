#ifndef GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESCOMMAND_H
#define GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESCOMMAND_H

#include "geodatapointmap_tinmanager.h"

#include <QUndoCommand>

class GeoDataPointmap::TINManager::RemoveTrianglesCommand : public QUndoCommand
{
public:
	RemoveTrianglesCommand(vtkCellArray* newPolys, TINManager* manager);

	void redo() override;
	void undo() override;

	void apply(vtkCellArray* cellArray);

	vtkSmartPointer<vtkCellArray> m_newCellArray;
	vtkSmartPointer<vtkCellArray> m_oldCellArray;

	TINManager* m_manager;
};

#endif // GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESCOMMAND_H
