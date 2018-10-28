#include "geodatapointmap_addpointsetreferencecommand.h"

GeoDataPointmap::AddPointSetReferenceCommand::AddPointSetReferenceCommand(double value, GeoDataPointmap* p) :
	QUndoCommand {GeoDataPointmap::tr("Select Refecence Point")},
	m_pointMap {p},
	m_value {value}
{}

void GeoDataPointmap::AddPointSetReferenceCommand::redo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meAddPoint;
	m_pointMap->m_newPointValue = m_value;
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
}

void GeoDataPointmap::AddPointSetReferenceCommand::undo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meAddPointSelectReferenceNotPossible;
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
}
