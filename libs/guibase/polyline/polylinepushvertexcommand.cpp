#include "polylinecontroller.h"
#include "polylinepushvertexcommand.h"

#include <misc/qundocommandhelper.h>


PolyLinePushVertexCommand::PolyLinePushVertexCommand(bool keyDown, const QPointF& point, PolyLineController* line) :
	PolyLinePushVertexCommand {tr("Add New Line Point"), keyDown, point, line}
{}

PolyLinePushVertexCommand::PolyLinePushVertexCommand(const QString& title, bool keyDown, const QPointF& point, PolyLineController* line) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_point {point},
	m_polyLine {line}
{}

void PolyLinePushVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	if (m_keyDown) {
		line.push_back(m_point);
		if (line.size() == 1) {
			line.push_back(m_point);
		}
	} else {
		line[line.size() - 1] = m_point;
	}
	m_polyLine->setPolyLine(line);
}

void PolyLinePushVertexCommand::undo()
{
	auto line = m_polyLine->polyLine();
	if (m_keyDown) {
		line.pop_back();
		if (line.size() == 1) {
			line.clear();
		}
	}
	m_polyLine->setPolyLine(line);
}

int PolyLinePushVertexCommand::id() const
{
	return iRIC::generateCommandId("PolyLinePushVertexCommand");
}

bool PolyLinePushVertexCommand::mergeWith(const QUndoCommand* other)
{
	auto comm = dynamic_cast<const PolyLinePushVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyLine != m_polyLine) {return false;}

	m_point = comm->m_point;
	return true;
}
