#ifndef GEODATANETCDFPROXY_DISPLAYSETTINGWIDGET_H
#define GEODATANETCDFPROXY_DISPLAYSETTINGWIDGET_H

#include "../geodatanetcdf.h"
#include "../geodatanetcdfproxy.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GeoDataNetcdfProxy_DisplaySettingWidget;
}

class DelegatedColorMapSettingEditWidget;

class GeoDataNetcdfProxy::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(GeoDataNetcdfProxy* proxy, QWidget *parent);
	~DisplaySettingWidget();
	GeoDataNetcdf::DisplaySettingWidget* displaySettingWidget() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

private:
	DelegatedColorMapSettingEditWidget* m_colorMapEditWidget;
	GeoDataNetcdfProxy* m_proxy;
	Ui::GeoDataNetcdfProxy_DisplaySettingWidget *ui;
};

#endif // GEODATANETCDFPROXY_DISPLAYSETTINGWIDGET_H
