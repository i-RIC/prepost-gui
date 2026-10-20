#ifndef GEODATARIVERSURVEY_EDITJMKDATACOMMAND_H
#define GEODATARIVERSURVEY_EDITJMKDATACOMMAND_H

#include "../geodatariversurvey.h"

#include <QPointer>
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
	// the window may be closed while this command remains in the undo stack
	QPointer<GeoDataRiverSurveyCrosssectionWindow> m_window;
};

#endif // GEODATARIVERSURVEY_EDITJMKDATACOMMAND_H
