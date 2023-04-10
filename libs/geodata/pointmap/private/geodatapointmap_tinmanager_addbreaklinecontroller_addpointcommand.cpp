#include "geodatapointmap_tinmanager_addbreaklinecontroller_addpointcommand.h"
#include "geodatapointmap_tinmanager_breakline.h"

#include <misc/qundocommandhelper.h>

GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand::AddPointCommand(bool click, bool removeLast, vtkIdType newPoint, AddBreakLineController* controller) :
	m_click {click},
	m_removeLast {removeLast},
	m_newPoint {newPoint},
	m_oldPoint {-1},
	m_breakLine {controller->m_breakLine}
{}

void GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand::redo()
{
	auto indices = m_breakLine->vertexIndices();
	if (m_removeLast) {
		m_oldPoint = *indices.rbegin();
		indices.pop_back();
	} else {
		m_oldPoint = -1;
	}
	if (m_newPoint >= 0) {
		if (! m_click) {
			indices.push_back(m_newPoint);
		}
		else {
			indices.push_back(m_newPoint);
			indices.push_back(m_newPoint);
		}
	}
	m_breakLine->setVertexIndices(indices);
}

void GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand::undo()
{
	auto indices = m_breakLine->vertexIndices();
	if (m_newPoint >= 0) {
		if (! m_click) {
			indices.pop_back();
		} else {
			indices.pop_back();
			indices.pop_back();
		}
	}
	if (m_removeLast) {
		indices.push_back(m_oldPoint);
	}
	m_breakLine->setVertexIndices(indices);
}

int GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand");
}

bool GeoDataPointmap::TINManager::AddBreakLineController::AddPointCommand::mergeWith(const QUndoCommand* other)
{
	if (m_click) {return false;}

	auto command = dynamic_cast<const AddPointCommand*>(other);
	if (command == 0) {return false;}
	if (m_breakLine != command->m_breakLine) {return false;}

	m_click = command->m_click;
	m_removeLast = command->m_removeLast;
	m_newPoint = command->m_newPoint;
	m_oldPoint = command->m_oldPoint;

	return true;
}
