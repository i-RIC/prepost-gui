#ifndef GEODATAPOINTPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINTPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapoint.h"
#include "../geodatapointproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPointProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPointProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPointProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPoint::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	GeoDataPointProxy* m_proxy;
	Ui::GeoDataPointProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINTPROXY_DISPLAYSETTINGWIDGET_H
