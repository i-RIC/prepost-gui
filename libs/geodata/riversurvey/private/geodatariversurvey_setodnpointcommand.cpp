#include "../geodatariversurveycrosssectionwindow.h"
#include "geodatariversurvey_setodnpointcommand.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>

GeoDataRiverSurvey::SetOdnPointCommand::SetOdnPointCommand(GeoDataRiverPathPoint* point, int pointId, int index, GeoDataRiverSurveyCrosssectionWindow* w) :
	m_newIndex {index},
	m_point {point},
	m_pointId {pointId},
	m_groupDataItem {w->groupDataItem()}
{
	m_oldIndex = point->odn().nb(m_pointId);
}

void GeoDataRiverSurvey::SetOdnPointCommand::redo()
{
	apply(m_newIndex);
}

void GeoDataRiverSurvey::SetOdnPointCommand::undo()
{
	apply(m_oldIndex);
}

void GeoDataRiverSurvey::SetOdnPointCommand::apply(int index)
{
	m_point->odn().setNb(m_pointId, index);

	m_groupDataItem->updateCrossectionWindows();
}
