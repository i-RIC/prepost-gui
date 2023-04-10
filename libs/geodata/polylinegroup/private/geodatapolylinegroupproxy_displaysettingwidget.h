#ifndef GEODATAPOLYLINEGROUPPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYLINEGROUPPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapolylinegroup.h"
#include "../geodatapolylinegroupproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolyLineGroupProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPolyLineGroupProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPolyLineGroupProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPolyLineGroup::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataPolyLineGroupProxy* m_proxy;
	Ui::GeoDataPolyLineGroupProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYLINEGROUPPROXY_DISPLAYSETTINGWIDGET_H
