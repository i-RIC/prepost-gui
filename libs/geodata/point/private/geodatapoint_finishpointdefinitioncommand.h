#ifndef GEODATAPOINT_FINISHPOINTDEFINITIONCOMMAND_H
#define GEODATAPOINT_FINISHPOINTDEFINITIONCOMMAND_H

#include "../geodatapoint.h"

#include <QUndoCommand>

class GeoDataPoint::FinishPointDefiningCommand : public QUndoCommand
{
public:
	FinishPointDefiningCommand(GeoDataPoint* point);

	void redo() override;
	void undo() override;

private:
	GeoDataPoint* m_point;
};

#endif // GEODATAPOINT_FINISHPOINTDEFINITIONCOMMAND_H
