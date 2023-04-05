#ifndef GEODATAPOINTGROUPPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINTGROUPPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapointgroup.h"
#include "../geodatapointgroupproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPointGroupProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPointGroupProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPointGroupProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPointGroup::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	GeoDataPointGroupProxy* m_proxy;
	Ui::GeoDataPointGroupProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINTGROUPPROXY_DISPLAYSETTINGWIDGET_H
