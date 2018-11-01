#ifndef GEODATARIVERSURVEY_DELETERIVERPATHPOINTCOMMAND_H
#define GEODATARIVERSURVEY_DELETERIVERPATHPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::DeleteRiverPathPointCommand : public QUndoCommand
{
public:
	DeleteRiverPathPointCommand(GeoDataRiverSurvey* rs);
	~DeleteRiverPathPointCommand();

	void redo() override;
	void undo() override;

private:
	QList<GeoDataRiverPathPoint*> m_deletedPoints;
	QList<GeoDataRiverPathPoint*> m_beforePoints;
	QList<bool> m_prevSkips;
	QList<bool> m_nextSkips;
	GeoDataRiverSurvey* m_rs;
	bool m_redoed;
};

#endif // GEODATARIVERSURVEY_DELETERIVERPATHPOINTCOMMAND_H
