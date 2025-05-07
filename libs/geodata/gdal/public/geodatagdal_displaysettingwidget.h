#ifndef GEODATAGDAL_DISPLAYSETTINGWIDGET_H
#define GEODATAGDAL_DISPLAYSETTINGWIDGET_H

#include "../private/geodatagdal_displaysetting.h"
#include "../geodatagdal.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataGdal_DisplaySettingWidget;
}

class GeoDataGdal::DisplaySettingWidget : public ModifyCommandWidget
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
	void setIsReferenceInformation(bool isReference);

private slots:
	void handleColorByValueToggle(bool toggled);

private:
	DisplaySetting* m_displaySetting;
	ModifyCommandWidget* m_colorMapWidget;

	Ui::GeoDataGdal_DisplaySettingWidget *ui;
};

#endif // GEODATAGDAL_DISPLAYSETTINGWIDGET_H
