#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowparticlesbasetopdataitem.h"
#include "../post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem_propertydialog.h"
#include "post2dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h"
#include "ui_post2dwindowparticlesbasevectorgroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>

Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::PropertyDialog(Post2dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_filteringEditWidget {new GeneralFilteringSettingEditWidget(this)},
	m_item {item},
	ui(new Ui::Post2dWindowParticlesBaseVectorGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);

	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_filteringEditWidget);

	auto gridTypeDataItem = item->gridTypeDataItem();
	auto particleData = item->particleData();

	std::map<std::string, QString> names;
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(particleData->GetPointData())) {
		auto caption = gridTypeDataItem->gridType()->solutionCaption(name);

		names.insert({name, caption});
	}
	ui->arrowsSettingWidget->setValueNames(names);

	auto colorMaps = m_item->topDataItem()->scalarGroupDataItem()->colorMapSettings();
	ui->arrowsSettingWidget->setColorMapSettings(colorMaps);

	ui->arrowsSettingWidget->setSetting(&m_item->m_setting.arrowsSetting);
	m_filteringEditWidget->setSetting(&m_item->m_setting.filteringSetting);
}

Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	return new UpdateSettingCommand(apply,
				ui->arrowsSettingWidget->createModifyCommand(),
				m_filteringEditWidget->createModifyCommand(),
				m_item);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);
	QDialog::accept();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
	m_applied = true;
}
