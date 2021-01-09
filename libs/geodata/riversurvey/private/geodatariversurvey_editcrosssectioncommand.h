#ifndef GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H

#include "../geodatarivercrosssection.h"
#include "../geodatariversurvey.h"

#include <QUndoCommand>

class PreProcessorGeoDataGroupDataItemInterface;

class GeoDataRiverSurvey::EditCrosssectionCommand : public QUndoCommand
{
public:
	const static int NO_SEL = -1;

	EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, int after_sel, const GeoDataRiverCrosssection::AltitudeList& before, int before_sel, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction = false, QUndoCommand* parentcommand = nullptr);

	void redo() override;
	void undo() override;

private:
	bool m_apply;
	bool m_first;
	bool m_tableaction;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverCrosssection::AltitudeList m_before;
	int m_beforeSelection;
	GeoDataRiverCrosssection::AltitudeList m_after;
	int m_afterSelection;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
	PreProcessorGeoDataGroupDataItemInterface* m_groupDataItem;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_EDITCROSSSECTIONCOMMAND_H
