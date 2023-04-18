#ifndef GEODATAPOLYLINE_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYLINE_DISPLAYSETTINGWIDGET_H

#include "../private/geodatapolyline_displaysetting.h"
#include "../geodatapolyline.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolyLine_DisplaySettingWidget;
}

class GeoDataPolyLine::DisplaySettingWidget : public ModifyCommandWidget
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

	Ui::GeoDataPolyLine_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYLINE_DISPLAYSETTINGWIDGET_H
