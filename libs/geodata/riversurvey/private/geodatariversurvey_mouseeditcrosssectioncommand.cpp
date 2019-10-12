#include "geodatariversurvey_mouseeditcrosssectioncommand.h"
#include "../geodatariversurveycrosssectionwindow.h"
#include "../geodatariversurveycrosssectionwindowgraphicsview.h"

#include <guicore/misc/qundocommandhelper.h>

GeoDataRiverSurvey::MouseEditCrosssectionCommand::MouseEditCrosssectionCommand(GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool dragging) :
	QUndoCommand {GeoDataRiverSurveyCrosssectionWindowGraphicsView::tr("Altitude Points Move")},
	m_dragging {dragging},
	m_point {p},
	m_before (before),
	m_after (after),
	m_window {w},
	m_rs {rs}
{}

void GeoDataRiverSurvey::MouseEditCrosssectionCommand::redo()
{
	m_point->crosssection().AltitudeInfo() = m_after;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (m_dragging) {
		m_window->updateView();
	} else {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrosssectionWindows();
	}
}

void GeoDataRiverSurvey::MouseEditCrosssectionCommand::undo()
{
	m_point->crosssection().AltitudeInfo() = m_before;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}

int GeoDataRiverSurvey::MouseEditCrosssectionCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurveyCrosssectionDragEdit");
}

bool GeoDataRiverSurvey::MouseEditCrosssectionCommand::mergeWith(const QUndoCommand* other)
{
	const MouseEditCrosssectionCommand* comm = dynamic_cast<const MouseEditCrosssectionCommand*>(other);
	if (comm == nullptr) {return false;}
	if (m_point != comm->m_point) {return false;}
	if (! m_dragging) {return false;}
	m_after = comm->m_after;
	m_dragging = comm->m_dragging;
	return true;
}
