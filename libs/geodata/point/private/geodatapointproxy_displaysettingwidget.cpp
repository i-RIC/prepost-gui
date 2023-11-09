#include "geodatapoint_impl.h"
#include "geodatapointproxy_displaysetting.h"
#include "geodatapointproxy_displaysettingwidget.h"
#include "geodatapointproxy_impl.h"
#include "../public/geodatapoint_displaysettingwidget.h"
#include "ui_geodatapointproxy_displaysettingwidget.h"

#include <guicore/post/post2d/base/post2dwindowgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataPointProxy::DisplaySettingWidget::DisplaySettingWidget(GeoDataPointProxy* proxy, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_proxy {proxy},
	ui(new Ui::GeoDataPointProxy_DisplaySettingWidget)
{
	ui->setupUi(this);
	ui->usePreCheckBox->hide();

	if (proxy->geoData()->gridAttribute()->isReferenceInformation()) {
		ui->displaySettingWidget->setIsReferenceInformation(true);
	}

	if (proxy->impl->m_displaySetting.usePreSetting) {
		auto geoData = dynamic_cast<GeoDataPoint*> (proxy->geoData());
		proxy->impl->m_displaySetting.displaySetting = geoData->impl->m_displaySetting;
	}
	setSetting(proxy->impl->m_displaySetting);
	ui->displaySettingWidget->setSetting(&proxy->impl->m_displaySetting.displaySetting);
}

GeoDataPointProxy::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete ui;
}

GeoDataPoint::DisplaySettingWidget* GeoDataPointProxy::DisplaySettingWidget::displaySettingWidget() const
{
	return ui->displaySettingWidget;
}

QUndoCommand* GeoDataPointProxy::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPointProxy::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting> (iRIC::generateCommandId("UsePre"), true, setting(), &m_proxy->impl->m_displaySetting));
	command->addCommand(ui->displaySettingWidget->createModifyCommand(apply));
	return command;
}

GeoDataPointProxy::DisplaySetting GeoDataPointProxy::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;
	setting.usePreSetting = ui->usePreCheckBox->isChecked();

	return setting;
}

void GeoDataPointProxy::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	ui->usePreCheckBox->setChecked(setting.usePreSetting);
}
