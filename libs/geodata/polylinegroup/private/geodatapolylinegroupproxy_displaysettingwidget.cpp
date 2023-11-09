#include "geodatapolylinegroup_impl.h"
#include "geodatapolylinegroupproxy_displaysetting.h"
#include "geodatapolylinegroupproxy_displaysettingwidget.h"
#include "geodatapolylinegroupproxy_impl.h"
#include "ui_geodatapolylinegroupproxy_displaysettingwidget.h"

#include <guicore/post/post2d/base/post2dwindowgeodatadataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingeditwidget.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataPolyLineGroupProxy::DisplaySettingWidget::DisplaySettingWidget(GeoDataPolyLineGroupProxy* proxy, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_colorMapEditWidget {nullptr},
	m_proxy {proxy},
	ui(new Ui::GeoDataPolyLineGroupProxy_DisplaySettingWidget)
{
	ui->setupUi(this);
	ui->usePreCheckBox->hide();

	if (proxy->geoData()->gridAttribute()->isReferenceInformation()) {
		ui->displaySettingWidget->setIsReferenceInformation(true);
	} else {
		auto colorMapWidget = proxy->geoData()->gridAttribute()->createColorMapSettingEditWidget(this);
		m_colorMapEditWidget = new DelegatedColorMapSettingEditWidget(this);
		m_colorMapEditWidget->setEditWidget(colorMapWidget);
		auto setting = proxy->geoDataDataItem()->colorMapSetting();
		m_colorMapEditWidget->setSetting(setting);

		ui->displaySettingWidget->setColorMapWidget(m_colorMapEditWidget);
	}

	if (proxy->impl->m_displaySetting.usePreSetting) {
		auto geoData = dynamic_cast<GeoDataPolyLineGroup*> (proxy->geoData());
		proxy->impl->m_displaySetting.displaySetting = geoData->impl->m_displaySetting;
	}
	setSetting(proxy->impl->m_displaySetting);
	ui->displaySettingWidget->setSetting(&proxy->impl->m_displaySetting.displaySetting);
}

GeoDataPolyLineGroupProxy::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete ui;
}

GeoDataPolyLineGroup::DisplaySettingWidget* GeoDataPolyLineGroupProxy::DisplaySettingWidget::displaySettingWidget() const
{
	return ui->displaySettingWidget;
}

QUndoCommand* GeoDataPolyLineGroupProxy::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPolyLineGroupProxy::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting> (iRIC::generateCommandId("UsePre"), true, setting(), &m_proxy->impl->m_displaySetting));
	command->addCommand(ui->displaySettingWidget->createModifyCommand(apply));
	if (m_colorMapEditWidget != nullptr) {
		command->addCommand(m_colorMapEditWidget->createModifyCommand(apply));
	}
	return command;
}

GeoDataPolyLineGroupProxy::DisplaySetting GeoDataPolyLineGroupProxy::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;
	setting.usePreSetting = ui->usePreCheckBox->isChecked();

	return setting;
}

void GeoDataPolyLineGroupProxy::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	ui->usePreCheckBox->setChecked(setting.usePreSetting);
}
