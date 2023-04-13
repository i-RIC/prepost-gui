#ifndef GEODATAPOLYLINEPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYLINEPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapolyline.h"
#include "../geodatapolylineproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolyLineProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPolyLineProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPolyLineProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPolyLine::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataPolyLineProxy* m_proxy;
	Ui::GeoDataPolyLineProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYLINEPROXY_DISPLAYSETTINGWIDGET_H
