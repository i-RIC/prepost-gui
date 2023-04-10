#ifndef GEODATARIVERSURVEYPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATARIVERSURVEYPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatariversurvey.h"
#include "../geodatariversurveyproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataRiverSurveyProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataRiverSurveyProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	DisplaySettingWidget(GeoDataRiverSurveyProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataRiverSurvey::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataRiverSurveyProxy* m_proxy;
	Ui::GeoDataRiverSurveyProxy_DisplaySettingWidget *ui;
};

#endif // GEODATARIVERSURVEYPROXY_DISPLAYSETTINGWIDGET_H
