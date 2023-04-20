#include "../post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowcellcontourgroupdataitem_setrangescommand.h"
#include "post3dwindowcellcontourgroupdataitem_propertydialog.h"
#include "post3dwindowcellcontourgroupdataitem_updateactorsettingcommand.h"
#include "ui_post3dwindowcellcontourgroupdataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>

#include <vtkStructuredGrid.h>

Post3dWindowCellContourGroupDataItem::PropertyDialog::PropertyDialog(Post3dWindowCellContourGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post3dWindowCellContourGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	ui->colorMapWidget->setSetting(&item->m_colorMapSetting);

	auto grid = vtkStructuredGrid::SafeDownCast(m_item->data()->data()->data());
	ui->rangeListWidget->setDimensions(grid->GetDimensions());
	ui->rangeListWidget->setRanges(item->ranges());
}

Post3dWindowCellContourGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post3dWindowCellContourGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto colorMapCommand = ui->colorMapWidget->createModifyCommand();
	auto rangeCommand = new SetRangesCommand(ui->rangeListWidget->ranges(), m_item);

	return new UpdateActorSettingCommand(apply, colorMapCommand, rangeCommand, m_item);
}

void Post3dWindowCellContourGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);

	QDialog::accept();
}

void Post3dWindowCellContourGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post3dWindowCellContourGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post3dWindowCellContourGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
	m_applied = true;
}
