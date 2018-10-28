#ifndef GEODATAPOINTMAP_REMOVETRIANGLESCOMMAND_H
#define GEODATAPOINTMAP_REMOVETRIANGLESCOMMAND_H

#include "../geodatapointmap.h"

#include <vtkCellArray.h>
#include <vtkSmartPointer.h>

#include <QUndoCommand>

class GeoDataPointmap::RemoveTrianglesCommand : public QUndoCommand
{
public:
	RemoveTrianglesCommand(vtkCellArray* newPolys, GeoDataPointmap* pointmap);

	void redo() override;
	void undo() override;

	void apply(vtkCellArray* cellArray);

	vtkSmartPointer<vtkCellArray> m_newCellArray;
	vtkSmartPointer<vtkCellArray> m_oldCellArray;

	GeoDataPointmap* m_pointmap;
};

#endif // GEODATAPOINTMAP_REMOVETRIANGLESCOMMAND_H
