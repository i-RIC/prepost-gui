#ifndef GEODATARIVERSURVEY_CHANGESELECTIONCOMMAND_H
#define GEODATARIVERSURVEY_CHANGESELECTIONCOMMAND_H

#include "../geodatariversurvey.h"

#include <QSet>
#include <QUndoCommand>

class GeoDataRiverPathPoint;

class GeoDataRiverSurvey::ChangeSelectionCommand : public QUndoCommand
{
public:
	ChangeSelectionCommand(GeoDataRiverSurvey* rs, MouseBoundingBox* box);

	void redo() override;
	void undo() override;

private:
	void applySelection(const QSet<GeoDataRiverPathPoint*>& selected);
	void buildSelectedPointsSet(QSet<GeoDataRiverPathPoint*>& set);

	QSet<GeoDataRiverPathPoint*> m_oldSelectedPoints;
	QSet<GeoDataRiverPathPoint*> m_newSelectedPoints;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_CHANGESELECTIONCOMMAND_H
