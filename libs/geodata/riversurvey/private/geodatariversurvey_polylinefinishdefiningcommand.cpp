#include "geodatariversurvey_impl.h"
#include "geodatariversurvey_polylinefinishdefiningcommand.h"

GeoDataRiverSurvey::PolyLineFinishDefiningCommand::PolyLineFinishDefiningCommand(GeoDataRiverSurvey* rs) :
	QUndoCommand( {GeoDataRiverSurvey::tr("Finish Defining Polyline")}),
	m_targetLine {rs->impl->m_activeLine},
	m_rs {rs}
{}

void GeoDataRiverSurvey::PolyLineFinishDefiningCommand::redo()
{
	m_rs->impl->m_activeLine = m_targetLine;
	m_rs->impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Normal;
	m_rs->impl->createModeUpdateMouseCursor(m_rs->graphicsView());
}

void GeoDataRiverSurvey::PolyLineFinishDefiningCommand::undo()
{
	m_rs->impl->m_activeLine = m_targetLine;
	m_rs->impl->m_createMouseEventMode = Impl::CreateMouseEventMode::Defining;
	m_rs->impl->createModeUpdateMouseCursor(m_rs->graphicsView());
}
