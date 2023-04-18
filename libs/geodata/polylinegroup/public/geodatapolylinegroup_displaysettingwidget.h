#ifndef GEODATAPOLYLINEGROUP_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYLINEGROUP_DISPLAYSETTINGWIDGET_H

#include "../geodatapolylinegroup.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolyLineGroup_DisplaySettingWidget;
}

class GeoDataPolyLineGroup::DisplaySettingWidget : public ModifyCommandWidget
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

	Ui::GeoDataPolyLineGroup_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYLINEGROUP_DISPLAYSETTINGWIDGET_H
