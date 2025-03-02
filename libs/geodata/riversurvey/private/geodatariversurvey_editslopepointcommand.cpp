#include "../geodatariversurveycrosssectionwindow.h"
#include "geodatariversurvey_editslopepointcommand.h"

#include <misc/qundocommandhelper.h>

GeoDataRiverSurvey::EditSlopePointCommand::EditSlopePointCommand(bool apply, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& newAlist, const GeoDataRiverCrosssection::AltitudeList& oldAlist, GeoDataRiverSurveyCrosssectionWindow* w) :
	QUndoCommand {},
	m_after {newAlist},
	m_before {oldAlist},
	m_apply {apply},
	m_point {p},
	m_window {w}
{}

void GeoDataRiverSurvey::EditSlopePointCommand::redo()
{
	apply(m_after);
}

void GeoDataRiverSurvey::EditSlopePointCommand::undo()
{
	apply(m_before);
}

int GeoDataRiverSurvey::EditSlopePointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurvey::EditSlopePointCommand");
}

bool GeoDataRiverSurvey::EditSlopePointCommand::mergeWith(const QUndoCommand *other)
{
	auto com = dynamic_cast<const EditSlopePointCommand*> (other);
	if (com == nullptr) {return false;}

	if (! m_apply) {return false;}
	if (m_point != com->m_point) {return false;}
	if (m_window != com->m_window) {return false;}

	m_apply = com->m_apply;
	m_after = com->m_after;

	return true;
}

void GeoDataRiverSurvey::EditSlopePointCommand::apply(const GeoDataRiverCrosssection::AltitudeList& alist)
{
	m_point->crosssection().AltitudeInfo() = alist;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	m_window->updateView();
	m_point->riverSurvey()->setMapped(false);
}
