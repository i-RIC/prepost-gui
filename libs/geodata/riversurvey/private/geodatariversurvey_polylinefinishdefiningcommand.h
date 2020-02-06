#ifndef GEODATARIVERSURVEY_POLYLINEFINISHDEFININGCOMMAND_H
#define GEODATARIVERSURVEY_POLYLINEFINISHDEFININGCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::PolyLineFinishDefiningCommand : public QUndoCommand
{
public:
	PolyLineFinishDefiningCommand(GeoDataRiverSurvey* rs);

	void redo() override;
	void undo() override;

private:
	PolyLineController* m_targetLine;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_POLYLINEFINISHDEFININGCOMMAND_H
