#ifndef GEODATAPOINTMAP_DELETEPOINTSCOMMAND_H
#define GEODATAPOINTMAP_DELETEPOINTSCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::DeletePointsCommand : public QUndoCommand
{
public:
	DeletePointsCommand(const QString& title, QVector<vtkIdType> deletedPoints, GeoDataPointmap* p);

	void redo() override;
	void undo() override;

private:
	vtkSmartPointer<vtkPoints> m_newPoints;
	vtkSmartPointer<vtkDataArray> m_newValues;

	vtkSmartPointer<vtkPoints> m_oldPoints;
	vtkSmartPointer<vtkDataArray> m_oldValues;

	QVector<vtkIdType> m_deletedPoints;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_DELETEPOINTSCOMMAND_H
