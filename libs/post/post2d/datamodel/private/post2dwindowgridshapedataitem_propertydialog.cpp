#include "../post2dwindowzonedataitem.h"
#include "post2dwindowgridshapedataitem_propertydialog.h"
#include "post2dwindowgridshapedataitem_updateactorsettingscommand.h"
#include "ui_post2dwindowgridshapedataitem_propertydialog.h"

#include <guicore/misc/vtkpointsetandvaluerangeset.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

Post2dWindowGridShapeDataItem::PropertyDialog::PropertyDialog(Post2dWindowGridShapeDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowGridShapeDataItem_PropertyDialog)
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

Post2dWindowGridShapeDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowGridShapeDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = ui->shapeEditWidget->createModifyCommand();
	return new UpdateActorSettingsCommand(apply, command, m_item);
}

void Post2dWindowGridShapeDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(), m_item);
	QDialog::accept();
}

void Post2dWindowGridShapeDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post2dWindowGridShapeDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowGridShapeDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item);
}
