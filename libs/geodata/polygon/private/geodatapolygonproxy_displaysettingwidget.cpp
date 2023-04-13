#include "geodatapolygon_impl.h"
#include "geodatapolygonproxy_displaysetting.h"
#include "geodatapolygonproxy_displaysettingwidget.h"
#include "geodatapolygonproxy_impl.h"
#include "ui_geodatapolygonproxy_displaysettingwidget.h"

#include <guicore/post/post2d/base/post2dwindowgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingeditwidget.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataPolygonProxy::DisplaySettingWidget::DisplaySettingWidget(GeoDataPolygonProxy* proxy, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_colorMapEditWidget {nullptr},
	m_proxy {proxy},
	ui(new Ui::GeoDataPolygonProxy_DisplaySettingWidget)
{
	ui->setupUi(this);

	auto colorMapWidget = proxy->geoData()->gridAttribute()->createColorMapSettingEditWidget(this);
	if (proxy->geoData()->gridAttribute()->isReferenceInformation()) {
		ui->displaySettingWidget->setIsReferenceInformation(true);
	} else {
		m_colorMapEditWidget = new DelegatedColorMapSettingEditWidget(this);
		m_colorMapEditWidget->setEditWidget(colorMapWidget);
		auto setting = proxy->geoDataDataItem()->colorMapSetting();
		m_colorMapEditWidget->setSetting(setting);

		ui->displaySettingWidget->setColorMapWidget(m_colorMapEditWidget);
	}

	if (proxy->impl->m_displaySetting.usePreSetting) {
		auto geoData = dynamic_cast<GeoDataPolygon*> (proxy->geoData());
		proxy->impl->m_displaySetting.displaySetting = geoData->impl->m_displaySetting;
	}
	setSetting(proxy->impl->m_displaySetting);
	ui->displaySettingWidget->setSetting(&proxy->impl->m_displaySetting.displaySetting);
}

GeoDataPolygonProxy::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete ui;
}

GeoDataPolygon::DisplaySettingWidget* GeoDataPolygonProxy::DisplaySettingWidget::displaySettingWidget() const
{
	return ui->displaySettingWidget;
}

QUndoCommand* GeoDataPolygonProxy::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPolygonProxy::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting> (iRIC::generateCommandId("UsePre"), true, setting(), &m_proxy->impl->m_displaySetting));
	command->addCommand(ui->displaySettingWidget->createModifyCommand(apply));
	if (m_colorMapEditWidget != nullptr) {
		command->addCommand(m_colorMapEditWidget->createModifyCommand(apply));
	}
	return command;
}

GeoDataPolygonProxy::DisplaySetting GeoDataPolygonProxy::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;
	setting.usePreSetting = ui->usePreCheckBox->isChecked();

	return setting;
}

void GeoDataPolygonProxy::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	ui->usePreCheckBox->setChecked(setting.usePreSetting);
}
