#include "../post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgroupdataitem_setfacescommand.h"
#include "post3dwindowcontourgroupdataitem_propertydialog.h"
#include "post3dwindowcontourgroupdataitem_updateactorsettingscommand.h"
#include "ui_post3dwindowcontourgroupdataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>

#include <vtkStructuredGrid.h>

Post3dWindowContourGroupDataItem::PropertyDialog::PropertyDialog(Post3dWindowContourGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post3dWindowContourGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	ui->colorMapWidget->setSetting(&item->m_colorMapSetting);

	auto grid = vtkStructuredGrid::SafeDownCast(m_item->data()->data()->data());
	ui->faceListWidget->setDimensions(grid->GetDimensions());
	ui->faceListWidget->setFaces(item->faces());
}

Post3dWindowContourGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post3dWindowContourGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto colorMapCommand = ui->colorMapWidget->createModifyCommand();
	auto faceCommand = new SetFacesCommand(ui->faceListWidget->faces(), m_item);

	return new UpdateActorSettingsCommand(apply, colorMapCommand, faceCommand, m_item);
}

void Post3dWindowContourGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);

	QDialog::accept();
}

void Post3dWindowContourGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post3dWindowContourGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post3dWindowContourGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
}
