#ifndef GEODATAGDALPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATAGDALPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatagdal.h"
#include "../geodatagdalproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataGdalProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataGdalProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataGdalProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataGdal::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataGdalProxy* m_proxy;
	Ui::GeoDataGdalProxy_DisplaySettingWidget *ui;
};

#endif // GEODATAGDALPROXY_DISPLAYSETTINGWIDGET_H
