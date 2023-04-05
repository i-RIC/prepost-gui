#ifndef GEODATAPOINTMAPPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINTMAPPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapointmap.h"
#include "../geodatapointmapproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPointmapProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPointmapProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPointmapProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPointmap::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataPointmapProxy* m_proxy;
	Ui::GeoDataPointmapProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINTMAPPROXY_DISPLAYSETTINGWIDGET_H
