#include "post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.h"
#include "ui_post2dwindowgeodatagroupdataitem_scalarstocolorseditdialog.h"
#include "../post2dwindowgridtypedataitem.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/iricundostack.h>

Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::ScalarsToColorsEditDialog(Post2dWindowGeoDataGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowGeoDataGroupDataItem_ScalarsToColorsEditDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &ScalarsToColorsEditDialog::handleButtonClick);

	auto widget = m_item->condition()->createColorMapSettingEditWidget(this);
	ui->widget->setEditWidget(widget);
}

Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::~ScalarsToColorsEditDialog()
{
	delete ui;
}

void Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::setSetting(DelegatedColorMapSettingContainer* setting)
{
	ui->widget->setSetting(setting);
}

QUndoCommand* Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::createModifyCommand(bool apply)
{
	auto typeDataItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (m_item->parent()->parent());
	return typeDataItem->createApplyColorMapSettingAndRenderCommand(m_item->condition()->name(), ui->widget->createModifyCommand(apply), apply);
}

void Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(), m_item);
	QDialog::accept();
}

void Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowGeoDataGroupDataItem::ScalarsToColorsEditDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item);
}
