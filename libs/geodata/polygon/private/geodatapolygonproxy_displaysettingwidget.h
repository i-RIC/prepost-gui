#ifndef GEODATAPOLYGONPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYGONPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapolygon.h"
#include "../geodatapolygonproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolygonProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPolygonProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPolygonProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPolygon::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataPolygonProxy* m_proxy;
	Ui::GeoDataPolygonProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYGONPROXY_DISPLAYSETTINGWIDGET_H
