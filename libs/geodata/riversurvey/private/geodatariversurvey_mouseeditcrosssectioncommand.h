#ifndef GEODATARIVERSURVEY_MOUSEEDITCROSSSECTIONCOMMAND_H
#define GEODATARIVERSURVEY_MOUSEEDITCROSSSECTIONCOMMAND_H

#include "../geodatariversurvey.h"

#include <QPointer>
#include <QUndoCommand>

class GeoDataRiverSurvey::MouseEditCrosssectionCommand : public QUndoCommand
{
public:
	MouseEditCrosssectionCommand(GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool dragging = false);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_dragging;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverCrosssection::AltitudeList m_before;
	GeoDataRiverCrosssection::AltitudeList m_after;
	// the window may be closed while this command remains in the undo stack
	QPointer<GeoDataRiverSurveyCrosssectionWindow> m_window;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERSURVEY_MOUSEEDITCROSSSECTIONCOMMAND_H
