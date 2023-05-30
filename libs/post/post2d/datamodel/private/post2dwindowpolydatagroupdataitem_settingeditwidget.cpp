#include "../post2dwindowgridtypedataitem.h"
#include "post2dwindowpolydatagroupdataitem_settingeditwidget.h"
#include "ui_post2dwindowpolydatagroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

Post2dWindowPolyDataGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowPolyDataGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowPolyDataGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto w = ui->widget;

	std::unordered_map<std::string, QString> names;
	auto gtype = item->gridTypeDataItem()->gridType();
	auto data = item->polyData();
	for (auto const& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data->GetPointData())) {
		auto caption = gtype->output(name)->caption();
		names.insert({name, caption});
	}
	w->setValueNames(names);

	w->setColorMapSettings(item->m_colorMapSettings);
	w->setSetting(&item->m_setting);
}

Post2dWindowPolyDataGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowPolyDataGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return ui->widget->createModifyCommand(apply);
}
