#ifndef GEODATARIVERSURVEY_EDITJMKDATACOMMAND_H
#define GEODATARIVERSURVEY_EDITJMKDATACOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::EditJmkDataCommand : public QUndoCommand
{
public:
	EditJmkDataCommand(const GeoDataRiverPathPointJmkData& after, const GeoDataRiverPathPointJmkData& before, GeoDataRiverPathPoint* point, GeoDataRiverSurveyCrosssectionWindow* w);

	void redo() override;
	void undo() override;

protected:
	GeoDataRiverPathPointJmkData m_after;
	GeoDataRiverPathPointJmkData m_before;

	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
};

#endif // GEODATARIVERSURVEY_EDITJMKDATACOMMAND_H
