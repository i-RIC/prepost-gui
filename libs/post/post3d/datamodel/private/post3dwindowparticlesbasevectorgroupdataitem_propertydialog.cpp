#include "../post3dwindowgridtypedataitem.h"
#include "../post3dwindowparticlesbasetopdataitem.h"
#include "../post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem_propertydialog.h"
#include "post3dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h"
#include "ui_post3dwindowparticlesbasevectorgroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>

Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::PropertyDialog(Post3dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_filteringEditWidget {new GeneralFilteringSettingEditWidget(this)},
	m_item {item},
	ui(new Ui::Post3dWindowParticlesBaseVectorGroupDataItem_PropertyDialog)
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

Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	return new UpdateSettingCommand(apply,
				ui->arrowsSettingWidget->createModifyCommand(),
				m_filteringEditWidget->createModifyCommand(),
				m_item);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);
	QDialog::accept();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post3dWindowParticlesBaseVectorGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
	m_applied = true;
}
