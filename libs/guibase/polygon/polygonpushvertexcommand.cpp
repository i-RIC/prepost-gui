#include "polygoncontroller.h"
#include "polygonpushvertexcommand.h"

#include <misc/qundocommandhelper.h>

#include <QPolygonF>

PolygonPushVertexCommand::PolygonPushVertexCommand(bool keyDown, const QPointF& point, PolygonController* polygon) :
	PolygonPushVertexCommand {tr("Add New Polygon Point"), keyDown, point, polygon}
{}

PolygonPushVertexCommand::PolygonPushVertexCommand(const QString& title, bool keyDown, const QPointF& point, PolygonController* polygon) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_point {point},
	m_polygon {polygon}
{}

void PolygonPushVertexCommand::redo()
{
	auto pol = m_polygon->polygon();

	if (m_keyDown) {
		if (pol.size() == 0) {
			for (int i = 0; i < 3; ++i) {
				pol.push_back(m_point);
			}
		} else {
			pol.insert(pol.begin() + pol.size() - 1, m_point);
		}
	}	else {
		pol[pol.size() - 2] = m_point;
	}
	m_polygon->setPolygon(pol);
}

void PolygonPushVertexCommand::undo()
{
	auto pol = m_polygon->polygon();
	if (pol.size() == 3) {
		pol.clear();
	} else {
		pol.erase(pol.begin() + pol.size() - 2);
	}
	m_polygon->setPolygon(pol);
}

int PolygonPushVertexCommand::id() const
{
	return iRIC::generateCommandId("PolygonPushVertexCommand");
}

bool PolygonPushVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolygonPushVertexCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polygon != comm->m_polygon ) {return false;}

	m_point = comm->m_point;
	return true;
}
