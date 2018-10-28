#ifndef GEODATAPOINTMAP_BREAKLINEADDCOMMAND_H
#define GEODATAPOINTMAP_BREAKLINEADDCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::BreakLineAddCommand : public QUndoCommand
{
public:
	BreakLineAddCommand(GeoDataPointmap* parent);

	void redo() override;
	void undo() override;

private:
	GeoDataPointmapBreakLine* m_breakLine;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_BREAKLINEADDCOMMAND_H
