#ifndef GEODATAPOLYGON_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYGON_DISPLAYSETTINGWIDGET_H

#include "../private/geodatapolygon_displaysetting.h"
#include "../geodatapolygon.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolygon_DisplaySettingWidget;
}

class GeoDataPolygon::DisplaySettingWidget : public ModifyCommandWidget
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

	Ui::GeoDataPolygon_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYGON_DISPLAYSETTINGWIDGET_H
