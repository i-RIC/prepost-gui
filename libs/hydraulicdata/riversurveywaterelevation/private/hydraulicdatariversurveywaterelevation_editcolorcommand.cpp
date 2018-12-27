#include "hydraulicdatariversurveywaterelevation_editcolorcommand.h"

#include <guicore/base/iricmainwindowinterface.h>

HydraulicDataRiverSurveyWaterElevation::EditColorCommand::EditColorCommand(const QColor& color, HydraulicDataRiverSurveyWaterElevation* we) :
	m_newColor {color},
	m_oldColor {we->color()},
	m_we {we}
{}

void HydraulicDataRiverSurveyWaterElevation::EditColorCommand::redo()
{
	m_we->setColor(m_newColor);
	m_we->iricMainWindow()->updateCrosssectionWindows();
}

void HydraulicDataRiverSurveyWaterElevation::EditColorCommand::undo()
{
	m_we->setColor(m_oldColor);
	m_we->iricMainWindow()->updateCrosssectionWindows();
}
