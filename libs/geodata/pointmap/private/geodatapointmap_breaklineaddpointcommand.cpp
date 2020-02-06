#include "geodatapointmap_breaklineaddpointcommand.h"

#include "../geodatapointmapbreakline.h"

#include <misc/qundocommandhelper.h>

GeoDataPointmap::BreakLineAddPointCommand::BreakLineAddPointCommand(bool preview, vtkIdType addedPoint, GeoDataPointmap* parent) :
	QUndoCommand {GeoDataPointmap::tr("Add Break Line Point")}
{
	m_preview = preview;
	m_addedPoint = addedPoint,
	m_breakLine = parent->m_activeBreakLine;
	m_pointMap = parent;
}

void GeoDataPointmap::BreakLineAddPointCommand::redo()
{
	QVector<vtkIdType> indices = m_breakLine->vertexIndices();
	if (indices.count() > 0) {
		m_oldPoint = indices.last();
		indices.pop_back();
	} else {
		m_oldPoint = - 1;
	}
	if (m_preview) {
		indices.push_back(m_addedPoint);
	} else {
		indices.push_back(m_addedPoint);
		indices.push_back(m_addedPoint);
	}
	m_breakLine->setVertexIndices(indices);
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

void GeoDataPointmap::BreakLineAddPointCommand::undo()
{
	QVector<vtkIdType> indices = m_breakLine->vertexIndices();
	if (m_preview) {
		indices.pop_back();
		if (m_oldPoint != - 1) {
			indices.push_back(m_oldPoint);
		}
		return;
	}
	if (indices.count() > 1) {
		indices.pop_back();
		indices.pop_back();
	}
	if (m_oldPoint != - 1) {
		indices.push_back(m_oldPoint);
	}
	m_breakLine->setVertexIndices(indices);
	m_pointMap->m_needRemeshing = true;
	m_pointMap->setMapped(false);
}

int GeoDataPointmap::BreakLineAddPointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPointmapBreakLineAddPoint");
}

bool GeoDataPointmap::BreakLineAddPointCommand::mergeWith(const QUndoCommand* other)
{
	const BreakLineAddPointCommand* comm = dynamic_cast<const BreakLineAddPointCommand*>(other);
	if (! m_preview) {return false;}
	if (comm == 0) {return false;}
	if (m_pointMap != comm->m_pointMap) {return false;}
	if (m_breakLine != comm->m_breakLine) {return false;}
	m_preview = comm->m_preview;
	m_addedPoint = comm->m_addedPoint;
	return true;
}
