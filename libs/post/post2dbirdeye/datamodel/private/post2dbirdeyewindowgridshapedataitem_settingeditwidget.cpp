#include "../post2dbirdeyewindowzonedataitem.h"
#include "post2dbirdeyewindowgridshapedataitem_impl.h"
#include "post2dbirdeyewindowgridshapedataitem_setting.h"
#include "post2dbirdeyewindowgridshapedataitem_settingeditwidget.h"
#include "ui_post2dbirdeyewindowgridshapedataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <guibase/vtkdatasetattributestool.h>
#include <misc/stringtool.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget::SettingEditWidget(Post2dBirdEyeWindowGridShapeDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dBirdEyeWindowGridShapeDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto grid = item->zoneDataItem()->dataContainer()->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(grid);
	if (sgrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}

	std::map<QString, std::string> captionMap;
	auto gType = item->zoneDataItem()->dataContainer()->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->GetPointData())) {
		auto cap = gType->output(name)->caption();
		captionMap.insert({cap, name});
	}
	for (const auto& pair : captionMap) {
		ui->elevationComboBox->addItem(pair.first);
		m_valueNames.push_back(pair.second);
	}

	setSetting(m_item->impl->m_setting);
}

Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting> (iRIC::generateCommandId("Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget"), apply, setting(), &m_item->impl->m_setting);
}

Post2dBirdEyeWindowGridShapeDataItem::Setting Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget::setting() const
{
	Setting ret;

	ret.elevationTarget = m_valueNames.at(ui->elevationComboBox->currentIndex()).c_str();
	ret.gridShape = ui->shapeEditWidget->setting();

	return ret;
}

void Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->shapeEditWidget->setSetting(setting.gridShape);
	auto it = std::find(m_valueNames.begin(), m_valueNames.end(), iRIC::toStr(setting.elevationTarget));
	if (it == m_valueNames.end()) {it = m_valueNames.begin();}

	ui->elevationComboBox->setCurrentIndex(it - m_valueNames.begin());
}

void Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget::setSetting(Setting* setting)
{
	setSetting(*setting);
	m_setting = setting;
}
