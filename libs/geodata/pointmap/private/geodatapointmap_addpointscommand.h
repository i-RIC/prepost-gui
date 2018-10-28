#ifndef GEODATAPOINTMAP_ADDPOINTSCOMMAND_H
#define GEODATAPOINTMAP_ADDPOINTSCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmapAddPointDialog;

class GeoDataPointmap::AddPointsCommand : public QUndoCommand
{
public:
	AddPointsCommand(GeoDataPointmap* p, GeoDataPointmapAddPointDialog* ip);

	void redo() override;
	void undo() override;

private:
	vtkSmartPointer<vtkPoints> newpoints;
	vtkSmartPointer<vtkDoubleArray> newvalues;

	vtkSmartPointer<vtkPoints> oldpoints;
	vtkSmartPointer<vtkDoubleArray> oldvalues;

	GeoDataPointmapAddPointDialog* m_iPoints;
	GeoDataPointmap* m_pmap;
};


#endif // GEODATAPOINTMAP_ADDPOINTSCOMMAND_H
