#include "../post2dwindowgridtypedataitem.h"
#include "post2dwindowpolydatagroupdataitem_impl.h"
#include "post2dwindowpolydatagroupdataitem_settingeditwidget.h"
#include "ui_post2dwindowpolydatagroupdataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4polydata2d.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowPolyDataGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowPolyDataGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowPolyDataGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto w = ui->widget;

	std::unordered_map<std::string, QString> names;
	auto gtype = item->gridTypeDataItem()->gridType();
	auto data = item->polyData()->vtkConcreteData()->concreteData();
	for (auto const& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data->GetCellData())) {
		auto caption = gtype->output(name)->caption();
		names.insert({name, caption});
	}
	w->setGridType(gtype);
	w->setValueNames(names);
	w->setColorMapSettings(item->impl->m_colorMapSettings);
	w->setSetting(&item->impl->m_setting);
}

Post2dWindowPolyDataGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowPolyDataGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return ui->widget->createModifyCommand(apply);
}
