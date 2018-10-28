#include "geodatapointmap_breaklinefinishdefinitioncommand.h"

#include "../geodatapointmapbreakline.h"

GeoDataPointmap::BreakLineFinishDefinitionCommand::BreakLineFinishDefinitionCommand(GeoDataPointmap* parent) :
	QUndoCommand {GeoDataPointmap::tr("Finish Defining Break Line")},
	m_breakLine {parent->m_activeBreakLine},
	m_pointMap {parent}
{}

void GeoDataPointmap::BreakLineFinishDefinitionCommand::redo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meNormal;
	m_breakLine->setActive(false);
	m_pointMap->m_activeBreakLine = 0;
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
	m_pointMap->updateActionStatus();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

void GeoDataPointmap::BreakLineFinishDefinitionCommand::undo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meBreakLineAddNotPossible;
	m_pointMap->m_activeBreakLine = m_breakLine;
	m_breakLine->setActive(true);
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
	m_pointMap->updateActionStatus();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}
