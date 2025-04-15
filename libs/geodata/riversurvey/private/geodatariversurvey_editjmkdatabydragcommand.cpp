#include "geodatariversurvey_editjmkdatabydragcommand.h"

#include <misc/qundocommandhelper.h>

GeoDataRiverSurvey::EditJmkDataByDragCommand::EditJmkDataByDragCommand(bool dragging, int index, bool right, const GeoDataRiverPathPointJmkData& after, const GeoDataRiverPathPointJmkData& before, GeoDataRiverPathPoint* point, GeoDataRiverSurveyCrosssectionWindow* w) :
	EditJmkDataCommand(after, before, point, w),
	m_dragging {dragging},
	m_index {index},
	m_right {right}
{}

int GeoDataRiverSurvey::EditJmkDataByDragCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurvey::EditJmkDataByDragCommand");
}

bool GeoDataRiverSurvey::EditJmkDataByDragCommand::mergeWith(const QUndoCommand *other)
{
	if (! m_dragging) {return false;}

	auto com = dynamic_cast<const EditJmkDataByDragCommand*> (other);
	if (com == nullptr) {return false;}
	if (m_index != com->m_index) {return false;}
	if (m_right != com->m_right) {return false;}
	if (m_point != com->m_point) {return false;}
	if (m_window != com->m_window) {return false;}

	m_after = com->m_after;
	m_dragging = com->m_dragging;

	return true;
}
