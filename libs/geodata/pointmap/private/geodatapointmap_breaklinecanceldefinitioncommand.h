#ifndef GEODATAPOINTMAP_BREAKLINECANCELDEFINITIONCOMMAND_H
#define GEODATAPOINTMAP_BREAKLINECANCELDEFINITIONCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::BreakLineCancelDefinitionCommand : public QUndoCommand
{
public:
	BreakLineCancelDefinitionCommand(GeoDataPointmap* parent);

	void redo() override;
	void undo() override;

private:
	GeoDataPointmapBreakLine* m_breakLine;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_BREAKLINECANCELDEFINITIONCOMMAND_H
