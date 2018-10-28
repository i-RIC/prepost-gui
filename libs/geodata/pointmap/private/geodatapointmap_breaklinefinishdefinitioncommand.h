#ifndef GEODATAPOINTMAP_BREAKLINEFINISHDEFINITIONCOMMAND_H
#define GEODATAPOINTMAP_BREAKLINEFINISHDEFINITIONCOMMAND_H

#include "../geodatapointmap.h"

#include <QUndoCommand>

class GeoDataPointmap::BreakLineFinishDefinitionCommand : public QUndoCommand
{
public:
	BreakLineFinishDefinitionCommand(GeoDataPointmap* parent);

	void redo() override;
	void undo() override;

private:
	GeoDataPointmapBreakLine* m_breakLine;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAP_BREAKLINEFINISHDEFINITIONCOMMAND_H
