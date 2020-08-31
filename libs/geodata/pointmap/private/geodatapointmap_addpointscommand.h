#ifndef GEODATAPOINTMAP_ADDPOINTSCOMMAND_H
#define GEODATAPOINTMAP_ADDPOINTSCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmapAddPointDialog;

class GeoDataPointmap::AddPointsCommand : public QUndoCommand
{
public:
	AddPointsCommand(const std::vector<QPointF>& points, const std::vector<double>& values, GeoDataPointmap* pm);

	void redo() override;
	void undo() override;

private:
	void apply(vtkPoints* points, vtkDoubleArray* vals);

	vtkSmartPointer<vtkPoints> m_newPoints;
	vtkSmartPointer<vtkDoubleArray> m_newValues;

	vtkSmartPointer<vtkPoints> m_oldPoints;
	vtkSmartPointer<vtkDoubleArray> m_oldValues;

	GeoDataPointmap* m_pointmap;
};

#endif // GEODATAPOINTMAP_ADDPOINTSCOMMAND_H
