#ifndef GEODATAPOINTMAP_BREAKLINEADDPOINTCOMMAND_H
#define GEODATAPOINTMAP_BREAKLINEADDPOINTCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::BreakLineAddPointCommand : public QUndoCommand
{
public:
	BreakLineAddPointCommand(bool preview, vtkIdType addedPoint, GeoDataPointmap* parent);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_preview;
	vtkIdType m_addedPoint;
	vtkIdType m_oldPoint;
	GeoDataPointmapBreakLine* m_breakLine;
	GeoDataPointmap* m_pointMap;
};
#endif // GEODATAPOINTMAP_BREAKLINEADDPOINTCOMMAND_H
