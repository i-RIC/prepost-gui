#ifndef GEODATAPOINTMAP_INTERPOLATELINEADDPOINTCOMMAND_H
#define GEODATAPOINTMAP_INTERPOLATELINEADDPOINTCOMMAND_H

#include "../geodatapointmap.h"

#include <QPointF>
#include <QUndoCommand>

class GeoDataPointmap::InterpolateLineAddPointCommand : public QUndoCommand
{
public:
	InterpolateLineAddPointCommand(bool keyDown, const double x, const double y, double zpos, GeoDataPointmap* ptmap);

	void redo() override;
	void undo() override;

	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GeoDataPointmap* m_pointMap;
	QPointF m_newPoint;
	double zposition;
	vtkSmartPointer<vtkVertex> vertex;
};
#endif // GEODATAPOINTMAP_INTERPOLATELINEADDPOINTCOMMAND_H
