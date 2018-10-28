#include "geodatapointmap_breaklinecanceldefinitioncommand.h"

#include "../geodatapointmapbreakline.h"

GeoDataPointmap::BreakLineCancelDefinitionCommand::BreakLineCancelDefinitionCommand(GeoDataPointmap* parent) :
	QUndoCommand {GeoDataPointmap::tr("Cancel Defining Break Line")},
	m_breakLine {parent->m_activeBreakLine},
	m_pointMap {parent}
{}

void GeoDataPointmap::BreakLineCancelDefinitionCommand::redo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meNormal;
	m_breakLine->setActive(false);
	m_breakLine->setHidden(true);
	m_pointMap->m_activeBreakLine = 0;
	m_pointMap->m_breakLines.removeOne(m_breakLine);
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
	m_pointMap->updateActionStatus();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

void GeoDataPointmap::BreakLineCancelDefinitionCommand::undo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meBreakLineAddNotPossible;
	m_pointMap->m_activeBreakLine = m_breakLine;
	m_breakLine->setActive(true);
	m_breakLine->setHidden(false);
	m_pointMap->m_breakLines.append(m_breakLine);
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
	m_pointMap->updateActionStatus();
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}
