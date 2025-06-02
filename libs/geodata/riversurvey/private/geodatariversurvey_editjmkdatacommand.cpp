#include "../geodatariversurveycrosssectionwindow.h"
#include "geodatariversurvey_editjmkdatacommand.h"

GeoDataRiverSurvey::EditJmkDataCommand::EditJmkDataCommand(const GeoDataRiverPathPointJmkData& after, const GeoDataRiverPathPointJmkData& before, GeoDataRiverPathPoint* point, GeoDataRiverSurveyCrosssectionWindow* w) :
	m_after {after},
	m_before {before},
	m_point {point},
	m_window {w}
{}

void GeoDataRiverSurvey::EditJmkDataCommand::redo()
{
	m_point->jmk() = m_after;
	m_window->updateRiverSurveys();
	m_point->riverSurvey()->setMapped(false);
}

void GeoDataRiverSurvey::EditJmkDataCommand::undo()
{
	m_point->jmk() = m_before;
	m_window->updateRiverSurveys();
	m_point->riverSurvey()->setMapped(false);
}
