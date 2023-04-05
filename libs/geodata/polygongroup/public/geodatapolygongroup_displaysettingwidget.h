#ifndef GEODATAPOLYGONGROUP_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYGONGROUP_DISPLAYSETTINGWIDGET_H

#include "../private/geodatapolygongroup_displaysetting.h"
#include "../geodatapolygongroup.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolygonGroup_DisplaySettingWidget;
}

class GeoDataPolygonGroup::DisplaySettingWidget : public ModifyCommandWidget
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

	Ui::GeoDataPolygonGroup_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYGONGROUP_DISPLAYSETTINGWIDGET_H
