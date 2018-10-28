#ifndef GEODATAPOINTMAP_ADDPOINTCOMMAND_H
#define GEODATAPOINTMAP_ADDPOINTCOMMAND_H

#include "../geodatapointmap.h"

#include <QPointF>
#include <QUndoCommand>

class GeoDataPointmap::AddPointCommand : public QUndoCommand
{
public:
	AddPointCommand(bool keyDown, const QPoint& point, GeoDataPointmap* ptmap);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GeoDataPointmap* m_pointMap;
	QPointF m_newPoint;
};
#endif // GEODATAPOINTMAP_ADDPOINTCOMMAND_H
