#include "../post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridattributeabstractcelldataitem_propertydialog.h"
#include "ui_post2dwindowgridattributeabstractcelldataitem_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::PropertyDialog(Post2dWindowGridAttributeAbstractCellGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowGridAttributeAbstractCellDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
}

Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::hideLineWidth()
{
	ui->lineWidthLabel->hide();
	ui->lineWidthSpinBox->hide();
}

ColorMapSettingEditWidgetI* Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::widget() const
{
	return dynamic_cast<ColorMapSettingEditWidgetI*> (ui->widgetContainer->widget());
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	ui->widgetContainer->setWidget(w);
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);
}

int Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::lineWidth() const
{
	return ui->lineWidthSpinBox->value();
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::setLineWidth(int lineWidth)
{
	ui->lineWidthSpinBox->setValue(lineWidth);
}

OpacityContainer Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::opacity() const
{
	return ui->transparencyWidget->opacity();
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::setOpacity(const OpacityContainer& opacity)
{
	return ui->transparencyWidget->setOpacity(opacity);
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::accept()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(false), m_item);
	QDialog::accept();
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::apply()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(true), m_item);
	m_applied = true;
}

QUndoCommand* Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto ret = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog"), apply);
	ret->addCommand(widget()->createModifyCommand());
	ret->addCommand(new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("Opacity"), apply, opacity(), &m_item->opacity()));
	IntContainer width {"lineWidth"};
	width.setValue(lineWidth());
	ret->addCommand(new ValueModifyCommmand<IntContainer>(iRIC::generateCommandId("LineWidth"), apply, width, &m_item->lineWidth()));

	return ret;
}
