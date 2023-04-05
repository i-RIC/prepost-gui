#ifndef GEODATARIVERSURVEY_DISPLAYSETTINGWIDGET_H
#define GEODATARIVERSURVEY_DISPLAYSETTINGWIDGET_H

#include "../private/geodatariversurvey_displaysetting.h"
#include "../geodatariversurvey.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataRiverSurvey_DisplaySettingWidget;
}

class GeoDataRiverSurvey::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	DisplaySettingWidget(QWidget *parent = nullptr);
	~DisplaySettingWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

	void setSetting(DisplaySetting* setting);
	void setColorMapWidget(ModifyCommandWidget* widget);

private:
	DisplaySetting* m_displaySetting;
	ModifyCommandWidget* m_colorMapWidget;

private:
	Ui::GeoDataRiverSurvey_DisplaySettingWidget *ui;
};

#endif // GEODATARIVERSURVEY_DISPLAYSETTINGWIDGET_H
