#ifndef GEODATARIVERSURVEY_SETDISPLAYSETTINGCOMMAND_H
#define GEODATARIVERSURVEY_SETDISPLAYSETTINGCOMMAND_H

#include "../geodatariversurvey.h"

#include <QUndoCommand>

class GeoDataRiverSurvey::SetDisplaySettingCommand : public QUndoCommand
{
public:
	SetDisplaySettingCommand(const GeoDataRiverSurveyDisplaySetting& setting, GeoDataRiverSurvey* s);

	void redo() override;
	void undo() override;

private:
	GeoDataRiverSurveyDisplaySetting m_newSetting;
	GeoDataRiverSurveyDisplaySetting m_oldSetting;

	GeoDataRiverSurvey* m_survey;
};

#endif // GEODATARIVERSURVEY_SETDISPLAYSETTINGCOMMAND_H
