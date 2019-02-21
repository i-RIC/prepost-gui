#ifndef GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H

#include "../geodatarivercrosssection.h"
#include "../geodatariversurvey.h"

#include <QUndoCommand>

class PreProcessorGeoDataGroupDataItemInterface;

class GeoDataRiverSurvey::EditCrosssectionCommand : public QUndoCommand
{
public:
	EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction = false, QUndoCommand* parentcommand = nullptr);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	bool m_first;
	bool m_tableaction;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverCrosssection::AltitudeList m_before;
	GeoDataRiverCrosssection::AltitudeList m_after;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
	PreProcessorGeoDataGroupDataItemInterface* m_groupDataItem;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H
