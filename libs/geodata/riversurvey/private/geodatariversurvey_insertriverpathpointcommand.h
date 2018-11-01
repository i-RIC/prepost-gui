#ifndef GEODATARIVERSURVEY_INSERTRIVERPATHPOINTCOMMAND_H
#define GEODATARIVERSURVEY_INSERTRIVERPATHPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::InsertRiverPathPointCommand : public QUndoCommand
{
public:
	InsertRiverPathPointCommand(bool apply, GeoDataRiverPathPoint* prev, GeoDataRiverPathPoint* newpoint, GeoDataRiverSurvey* rs);
	~InsertRiverPathPointCommand();

	void redo() override;
	void undo() override;

private:
	GeoDataRiverPathPoint* m_previousPoint;
	GeoDataRiverPathPoint* m_newPoint;
	GeoDataRiverSurvey* m_rs;
	bool m_apply;
	bool m_redoed;
};

#endif // GEODATARIVERSURVEY_INSERTRIVERPATHPOINTCOMMAND_H
