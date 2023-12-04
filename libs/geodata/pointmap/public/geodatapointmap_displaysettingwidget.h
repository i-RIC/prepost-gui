#ifndef GEODATAPOINTMAP_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINTMAP_DISPLAYSETTINGWIDGET_H

#include "../private/geodatapointmap_displaysetting.h"
#include "../geodatapointmap.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPointmap_DisplaySettingWidget;
}

class GeoDataPointmap::DisplaySettingWidget : public ModifyCommandWidget
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
	void setPolygonColorMapWidget(ModifyCommandWidget* widget);

private:
	DisplaySetting* m_displaySetting;
	ModifyCommandWidget* m_colorMapWidget;
	ModifyCommandWidget* m_polygonColorMapWidget;

	Ui::GeoDataPointmap_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINTMAP_DISPLAYSETTINGWIDGET_H
