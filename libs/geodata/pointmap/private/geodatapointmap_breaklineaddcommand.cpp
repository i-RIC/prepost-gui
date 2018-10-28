#include "geodatapointmap_breaklineaddcommand.h"

#include "../geodatapointmapbreakline.h"

GeoDataPointmap::BreakLineAddCommand::BreakLineAddCommand(GeoDataPointmap* parent) :
	m_breakLine {new GeoDataPointmapBreakLine(parent)},
	m_pointMap {parent}
{}

void GeoDataPointmap::BreakLineAddCommand::redo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meBreakLineAddNotPossible;
	m_breakLine->setActive(true);
	m_pointMap->m_activeBreakLine = m_breakLine;
	m_pointMap->m_breakLines.append(m_breakLine);
	m_pointMap->assignActorZValues(m_pointMap->m_zDepthRange);
	m_pointMap->updateActionStatus();
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

void GeoDataPointmap::BreakLineAddCommand::undo()
{
	m_pointMap->m_mouseEventMode = GeoDataPointmap::meNormal;
	m_breakLine->setActive(false);
	m_pointMap->m_activeBreakLine = 0;
	m_pointMap->m_breakLines.removeOne(m_breakLine);
	m_pointMap->updateActionStatus();
	m_pointMap->updateMouseCursor(m_pointMap->graphicsView());
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}
