#include "../post3dwindowzonedataitem.h"
#include "post3dwindowgridshapedataitem_propertydialog.h"
#include "post3dwindowgridshapedataitem_updateactorsettingscommand.h"
#include "ui_post3dwindowgridshapedataitem_propertydialog.h"

#include <guicore/misc/vtkpointsetandvaluerangeset.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

Post3dWindowGridShapeDataItem::PropertyDialog::PropertyDialog(Post3dWindowGridShapeDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post3dWindowGridShapeDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);

	auto grid = item->zoneDataItem()->dataContainer()->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(grid);
	if (sgrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}

	ui->shapeEditWidget->setSetting(&item->m_setting);
}

Post3dWindowGridShapeDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post3dWindowGridShapeDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = ui->shapeEditWidget->createModifyCommand();
	return new UpdateActorSettingsCommand(apply, command, m_item);
}

void Post3dWindowGridShapeDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(), m_item);
	QDialog::accept();
}

void Post3dWindowGridShapeDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post3dWindowGridShapeDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post3dWindowGridShapeDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item);
}
