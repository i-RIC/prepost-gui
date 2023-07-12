#include "../post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem_settingeditwidget.h"
#include "ui_post2dwindowparticlesbasescalargroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowParticlesBaseScalarGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowParticlesBaseScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto w = ui->widget;

	std::unordered_map<std::string, QString> names;
	auto gtype = item->gridTypeDataItem()->gridType();
	auto data = item->particleData();
	for (auto const& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data->GetPointData())) {
		auto caption = gtype->output(name)->caption();
		names.insert({name, caption});
	}
	w->setGridType(gtype);
	w->setValueNames(names);
	w->setColorMapSettings(item->colorMapSettings());
	w->setSetting(&item->m_setting);
}

Post2dWindowParticlesBaseScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowParticlesBaseScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return ui->widget->createModifyCommand(apply);
}
