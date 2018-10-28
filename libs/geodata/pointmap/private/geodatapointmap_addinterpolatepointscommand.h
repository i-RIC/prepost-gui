#ifndef GEODATAPOINTMAP_ADDINTERPOLATEPOINTSCOMMAND_H
#define GEODATAPOINTMAP_ADDINTERPOLATEPOINTSCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmapInterpolatePoints;

class GeoDataPointmap::AddInterpolatePointsCommand : public QUndoCommand
{
public:
	AddInterpolatePointsCommand(GeoDataPointmap* p, GeoDataPointmapInterpolatePoints* ip);

	void redo() override;
	void undo() override;

private:
	GeoDataPointmap* m_pmap;
	GeoDataPointmapInterpolatePoints* m_iPoints;
	vtkSmartPointer<vtkPoints> newpoints;
	vtkSmartPointer<vtkDoubleArray> newvalues;
	vtkSmartPointer<vtkPoints> oldpoints;
	vtkSmartPointer<vtkDoubleArray> oldvalues;
};

#endif // GEODATAPOINTMAP_ADDINTERPOLATEPOINTSCOMMAND_H
