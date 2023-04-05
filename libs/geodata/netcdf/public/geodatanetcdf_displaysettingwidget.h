#ifndef GEODATANETCDF_DISPLAYSETTINGWIDGET_H
#define GEODATANETCDF_DISPLAYSETTINGWIDGET_H

#include "../private/geodatanetcdf_displaysetting.h"
#include "../geodatanetcdf.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataNetcdf_DisplaySettingWidget;
}

class GeoDataNetcdf::DisplaySettingWidget : public ModifyCommandWidget
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

	Ui::GeoDataNetcdf_DisplaySettingWidget *ui;
};

#endif // GEODATANETCDF_DISPLAYSETTINGWIDGET_H
