#ifndef GEODATAPOLYGONGROUPPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAPOLYGONGROUPPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatapolygongroup.h"
#include "../geodatapolygongroupproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataPolygonGroupProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataPolygonGroupProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataPolygonGroupProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataPolygonGroup::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataPolygonGroupProxy* m_proxy;
	Ui::GeoDataPolygonGroupProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAPOLYGONGROUPPROXY_DISPLAYSETTINGWIDGET_H
