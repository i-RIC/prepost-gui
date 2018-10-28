#ifndef GEODATAPOINTMAP_EDITPOINTSCOMMAND_H
#define GEODATAPOINTMAP_EDITPOINTSCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::EditPointsCommand : public QUndoCommand
{
public:
	EditPointsCommand(double value, QVector<vtkIdType> editedPoints, GeoDataPointmap* p);

	void redo() override;
	void undo() override;

private:
	vtkSmartPointer<vtkDataArray> m_newValues;
	vtkSmartPointer<vtkDataArray> m_oldValues;

	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_EDITPOINTSCOMMAND_H
