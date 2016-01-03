#ifndef GEODATARIVERSURVEYPROXY_SETSETTINGCOMMAND_H
#define GEODATARIVERSURVEYPROXY_SETSETTINGCOMMAND_H

#include "../geodatariversurveyproxy.h"

#include <QUndoCommand>

class GeoDataRiverSurveyProxy::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GeoDataRiverSurveyDisplaySetting& setting, GeoDataRiverSurveyProxy* proxy);

	void redo() override;
	void undo() override;

private:
	GeoDataRiverSurveyDisplaySetting m_newSetting;
	GeoDataRiverSurveyDisplaySetting m_oldSetting;

	GeoDataRiverSurveyProxy* m_proxy;
};

#endif // GEODATARIVERSURVEYPROXY_SETSETTINGCOMMAND_H
