#include "geodatapointmap_interpolatelineaddpointcommand.h"

#include <vtkDoubleArray.h>

GeoDataPointmap::InterpolateLineAddPointCommand::InterpolateLineAddPointCommand(double x, double y, double val, bool newFlag, GeoDataPointmap* map) :
	QUndoCommand {GeoDataPointmap::tr("Add New Interp Line Point")},
	m_newPoint {x, y},
	m_newVal {val},
	m_newFlag {newFlag},
	m_pointMap {map}
{}

void GeoDataPointmap::InterpolateLineAddPointCommand::redo()
{
	auto line = m_pointMap->m_interpolatePointsController.polyLine();
	line.push_back(m_newPoint);
	m_pointMap->m_interpolatePointsController.setPolyLine(line);

	m_pointMap->m_interpolateValues.push_back(m_newVal);
	m_pointMap->m_interpolateNewFlags.push_back(m_newFlag);
}

void GeoDataPointmap::InterpolateLineAddPointCommand::undo()
{
	auto line = m_pointMap->m_interpolatePointsController.polyLine();
	line.pop_back();
	m_pointMap->m_interpolatePointsController.setPolyLine(line);

	m_pointMap->m_interpolateValues.pop_back();
	m_pointMap->m_interpolateNewFlags.pop_back();
}
