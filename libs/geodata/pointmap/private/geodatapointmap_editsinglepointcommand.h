#ifndef GEODATAPOINTMAP_EDITSINGLEPOINTCOMMAND_H
#define GEODATAPOINTMAP_EDITSINGLEPOINTCOMMAND_H

#include "../geodatapointmap.h"

#include <QPointF>
#include <QUndoCommand>

class GeoDataPointmap::EditSinglePointCommand : public QUndoCommand
{
public:
	EditSinglePointCommand(double x, double y, double value, vtkIdType editedPoint, GeoDataPointmap* p);

	void redo() override;
	void undo() override;

private:
	void applyPoint(const QPointF& p, double v);

	QPointF m_newPoint;
	double m_newValue;

	QPointF m_oldPoint;
	double m_oldValue;

	vtkIdType m_editedPoint;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_EDITSINGLEPOINTCOMMAND_H
