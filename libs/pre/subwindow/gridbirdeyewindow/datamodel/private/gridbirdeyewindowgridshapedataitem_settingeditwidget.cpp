#include "../../../../datamodel/preprocessorgridtypedataitem.h"
#include "../gridbirdeyewindowzonedataitem.h"
#include "gridbirdeyewindowgridshapedataitem_impl.h"
#include "gridbirdeyewindowgridshapedataitem_setting.h"
#include "gridbirdeyewindowgridshapedataitem_settingeditwidget.h"
#include "ui_gridbirdeyewindowgridshapedataitem_settingeditwidget.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <guibase/vtkdatasetattributestool.h>
#include <misc/stringtool.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

GridBirdEyeWindowGridShapeDataItem::SettingEditWidget::SettingEditWidget(GridBirdEyeWindowGridShapeDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::GridBirdEyeWindowGridShapeDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto grid = item->zoneDataItem()->grid();
	if (grid == nullptr) {return;}

	auto grid2 = grid->grid();
	auto sgrid = dynamic_cast<v4Structured2dGrid*> (grid2);
	if (sgrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}

	std::map<QString, std::string> captionMap;
	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (item->dataModel());
	auto gType = model->gridTypeDataItem()->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid2->vtkData()->data()->GetPointData())) {
		auto cap = gType->gridAttribute(name)->caption();
		captionMap.insert({cap, name});
	}
	for (const auto& pair : captionMap) {
		ui->elevationComboBox->addItem(pair.first);
		m_valueNames.push_back(pair.second);
	}

	setSetting(m_item->impl->m_setting);
}

GridBirdEyeWindowGridShapeDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* GridBirdEyeWindowGridShapeDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting> (iRIC::generateCommandId("GridBirdEyeWindowGridShapeDataItem::SettingEditWidget"), apply, setting(), &m_item->impl->m_setting);
}

GridBirdEyeWindowGridShapeDataItem::Setting GridBirdEyeWindowGridShapeDataItem::SettingEditWidget::setting() const
{
	Setting ret;

	ret.elevationTarget = m_valueNames.at(ui->elevationComboBox->currentIndex()).c_str();
	ret.gridShape = ui->shapeEditWidget->setting();

	return ret;
}

void GridBirdEyeWindowGridShapeDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->shapeEditWidget->setSetting(setting.gridShape);
	auto it = std::find(m_valueNames.begin(), m_valueNames.end(), iRIC::toStr(setting.elevationTarget));
	if (it == m_valueNames.end()) {it = m_valueNames.begin();}

	ui->elevationComboBox->setCurrentIndex(it - m_valueNames.begin());
}

void GridBirdEyeWindowGridShapeDataItem::SettingEditWidget::setSetting(Setting* setting)
{
	setSetting(*setting);
	m_setting = setting;
}
