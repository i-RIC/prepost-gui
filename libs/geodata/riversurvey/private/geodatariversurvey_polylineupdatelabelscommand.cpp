#include "geodatariversurvey_impl.h"
#include "geodatariversurvey_polylineupdatelabelscommand.h"
#include "../geodatariversurvey.h"

GeoDataRiverSurvey::PolyLineUpdateLabelsCommand::PolyLineUpdateLabelsCommand(QUndoCommand* com, GeoDataRiverSurvey* rs) :
	m_command {com},
	m_rs {rs}
{}

void GeoDataRiverSurvey::PolyLineUpdateLabelsCommand::redo()
{
	m_command->redo();
	m_rs->impl->updateLabelsAndSplines();
}

void GeoDataRiverSurvey::PolyLineUpdateLabelsCommand::undo()
{
	m_command->undo();
	m_rs->impl->updateLabelsAndSplines();
}

int GeoDataRiverSurvey::PolyLineUpdateLabelsCommand::id() const
{
	return m_command->id();
}

bool GeoDataRiverSurvey::PolyLineUpdateLabelsCommand::mergeWith(const QUndoCommand *other)
{
	auto c = dynamic_cast<const PolyLineUpdateLabelsCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
