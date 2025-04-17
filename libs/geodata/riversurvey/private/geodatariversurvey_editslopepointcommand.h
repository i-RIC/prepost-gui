#ifndef GEODATARIVERSURVEY_EDITSLOPEPOINTCOMMAND_H
#define GEODATARIVERSURVEY_EDITSLOPEPOINTCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::EditSlopePointCommand : public QUndoCommand
{
public:
	EditSlopePointCommand(bool apply, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& newAlist, const GeoDataRiverCrosssection::AltitudeList& oldAlist, GeoDataRiverSurveyCrosssectionWindow* w);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	void apply(const GeoDataRiverCrosssection::AltitudeList& alist, const GeoDataRiverPathPointOdnData& odn);

	GeoDataRiverCrosssection::AltitudeList m_after;
	GeoDataRiverPathPointOdnData m_afterOdn;

	GeoDataRiverCrosssection::AltitudeList m_before;
	GeoDataRiverPathPointOdnData m_beforeOdn;

	bool m_apply;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
};

#endif // GEODATARIVERSURVEY_EDITSLOPEPOINTCOMMAND_H
