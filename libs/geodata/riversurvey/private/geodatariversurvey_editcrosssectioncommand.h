#ifndef GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H

#include "../geodatarivercrosssection.h"
#include "../geodatariversurvey.h"

#include <QUndoCommand>

class PreProcessorGeoDataGroupDataItemI;

class GeoDataRiverSurvey::EditCrosssectionCommand : public QUndoCommand
{
public:
	const static int NO_SEL = -1;

	EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, int after_sel, const GeoDataRiverCrosssection::AltitudeList& before, int before_sel, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction = false, QUndoCommand* parentcommand = nullptr);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;
	bool m_tableaction;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverCrosssection::AltitudeList m_before;
	GeoDataRiverPathPointOdnData m_beforeOdn;
	int m_beforeSelection;
	GeoDataRiverCrosssection::AltitudeList m_after;
	GeoDataRiverPathPointOdnData m_afterOdn;
	int m_afterSelection;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
	PreProcessorGeoDataGroupDataItemI* m_groupDataItem;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H
