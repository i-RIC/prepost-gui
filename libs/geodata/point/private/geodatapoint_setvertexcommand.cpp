#include "geodatapoint_impl.h"
#include "geodatapoint_setvertexcommand.h"

#include <misc/qundocommandhelper.h>

GeoDataPoint::SetVertexCommand::SetVertexCommand(bool keyDown, const QPointF& p, GeoDataPoint* point) :
	QUndoCommand(GeoDataPoint::tr("Set vertex")),
	m_position {p},
	m_keyDown {keyDown},
	m_point {point}
{}

void GeoDataPoint::SetVertexCommand::redo()
{
	m_point->setPoint(m_position);
	m_point->updateScalarValues();
	m_point->updateActorSettings();
}

void GeoDataPoint::SetVertexCommand::undo()
{
	m_point->impl->m_pointController.clear();
}

int GeoDataPoint::SetVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPointSetVertex");
}

bool GeoDataPoint::SetVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const SetVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	m_position = comm->m_position;
	return true;
}
