#include "../post2dwindownodescalargrouptopdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodescalargroupdataitem_propertydialog.h"
#include "post2dwindownodescalargroupdataitem_updateactorsettingscommand.h"
#include "ui_post2dwindownodescalargroupdataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

Post2dWindowNodeScalarGroupDataItem::PropertyDialog::PropertyDialog(Post2dWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowNodeScalarGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	auto container = item->topDataItem()->zoneDataItem()->dataContainer();
	if (! container->IBCExists()) {
		ui->rangeWidget->disableActive();
	}

	auto data = container->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(data);
	if (sgrid == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dims[3];
		sgrid->GetDimensions(dims);
		ui->rangeWidget->setDimensions(dims[0], dims[1]);
	}

	ui->colorMapWidget->setSetting(&item->m_setting.colorMapSetting);
	ui->rangeWidget->setSetting(&item->m_setting.regionSetting);
	ui->opacityWidget->setOpacityPercent(item->m_setting.opacity);
}

Post2dWindowNodeScalarGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeScalarGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeScalarGroupDataItem::SetProperty"), apply);
	command->addCommand(ui->colorMapWidget->createModifyCommand());
	command->addCommand(ui->rangeWidget->createModifyCommand());

	OpacityContainer o;
	o.setValue(ui->opacityWidget->opacityPercent());
	command->addCommand(new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("ModifyOpacity"), apply, o, &m_item->m_setting.opacity));

	return new UpdateActorSettingsCommand(apply, command, m_item);
}

void Post2dWindowNodeScalarGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);

	QDialog::accept();
}

void Post2dWindowNodeScalarGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dWindowNodeScalarGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowNodeScalarGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
}
