#include "geodatapointgroup_displaysettingwidget.h"
#include "geodatapointgroup_impl.h"
#include "geodatapointgroupproxy_displaysetting.h"
#include "geodatapointgroupproxy_displaysettingwidget.h"
#include "geodatapointgroupproxy_impl.h"
#include "ui_geodatapointgroupproxy_displaysettingwidget.h"

#include <guicore/post/post2d/base/post2dwindowgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataPointGroupProxy::DisplaySettingWidget::DisplaySettingWidget(GeoDataPointGroupProxy* proxy, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_proxy {proxy},
	ui(new Ui::GeoDataPointGroupProxy_DisplaySettingWidget)
{
	ui->setupUi(this);

	if (proxy->geoData()->gridAttribute()->isReferenceInformation()) {
		ui->displaySettingWidget->setIsReferenceInformation(true);
	}

	if (proxy->impl->m_displaySetting.usePreSetting) {
		auto geoData = dynamic_cast<GeoDataPointGroup*> (proxy->geoData());
		proxy->impl->m_displaySetting.displaySetting = geoData->impl->m_displaySetting;
	}
	setSetting(proxy->impl->m_displaySetting);
	ui->displaySettingWidget->setSetting(&proxy->impl->m_displaySetting.displaySetting);
}

GeoDataPointGroupProxy::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete ui;
}

GeoDataPointGroup::DisplaySettingWidget* GeoDataPointGroupProxy::DisplaySettingWidget::displaySettingWidget() const
{
	return ui->displaySettingWidget;
}

QUndoCommand* GeoDataPointGroupProxy::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPointGroupProxy::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting> (iRIC::generateCommandId("UsePre"), true, setting(), &m_proxy->impl->m_displaySetting));
	command->addCommand(ui->displaySettingWidget->createModifyCommand(apply));
	return command;
}

GeoDataPointGroupProxy::DisplaySetting GeoDataPointGroupProxy::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;
	setting.usePreSetting = ui->usePreCheckBox->isChecked();

	return setting;
}

void GeoDataPointGroupProxy::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	ui->usePreCheckBox->setChecked(setting.usePreSetting);
}
