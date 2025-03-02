#ifndef GEODATARIVERSURVEY_EDITSLOPEPOINTCOMMAND_H
#define GEODATARIVERSURVEY_EDITSLOPEPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::EditSlopePointCommand : public QUndoCommand
{
public:
	EditSlopePointCommand(bool apply, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& alist, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow* w);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	void apply(const GeoDataRiverCrosssection::AltitudeList& alist);

	GeoDataRiverCrosssection::AltitudeList m_after;
	GeoDataRiverCrosssection::AltitudeList m_before;

	bool m_apply;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
};

#endif // GEODATARIVERSURVEY_EDITSLOPEPOINTCOMMAND_H
