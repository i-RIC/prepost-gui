#include "hydraulicdatariversurveywaterelevation_editcaptioncommand.h"

#include <guicore/base/iricmainwindowinterface.h>

HydraulicDataRiverSurveyWaterElevation::EditCaptionCommand::EditCaptionCommand(const QString& caption, HydraulicDataRiverSurveyWaterElevation* we) :
	m_newCaption {caption},
	m_oldCaption {we->caption()},
	m_we {we}
{}

void HydraulicDataRiverSurveyWaterElevation::EditCaptionCommand::redo()
{
	m_we->setCaption(m_newCaption);
	m_we->iricMainWindow()->updateCrosssectionWindows();
}

void HydraulicDataRiverSurveyWaterElevation::EditCaptionCommand::undo()
{
	m_we->setCaption(m_oldCaption);
	m_we->iricMainWindow()->updateCrosssectionWindows();
}
