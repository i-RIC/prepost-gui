#include "../preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridattributeabstractcelldataitem_propertydialog.h"
#include "ui_preprocessorgridattributeabstractcelldataitem_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::PropertyDialog(PreProcessorGridAttributeAbstractCellGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::PreProcessorGridAttributeAbstractCellDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
}

PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::hideLineWidth()
{
	ui->gridCellSettingWidget->hideLineWidth();
}

ColorMapSettingEditWidgetI* PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::widget() const
{
	return dynamic_cast<ColorMapSettingEditWidgetI*> (ui->widgetContainer->widget());
}

void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	ui->widgetContainer->setWidget(w);
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);
}


void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::setSetting(GridAttributeCellSetting* setting)
{
	ui->gridCellSettingWidget->setSetting(setting);
}

void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::accept()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(false), m_item);
	QDialog::accept();
}

void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::apply()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(true), m_item);
	m_applied = true;
}

QUndoCommand* PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto ret = new MergeSupportedListCommand(iRIC::generateCommandId("PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog"), apply);
	ret->addCommand(widget()->createModifyCommand());
	ret->addCommand(ui->gridCellSettingWidget->createModifyCommand(apply));

	return ret;
}
