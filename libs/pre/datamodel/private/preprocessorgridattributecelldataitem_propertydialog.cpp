#include "../preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributecelldataitem_propertydialog.h"
#include "ui_preprocessorgridattributecelldataitem_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>

PreProcessorGridAttributeCellDataItem::PropertyDialog::PropertyDialog(PreProcessorGridAttributeCellGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::PreProcessorGridAttributeCellDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
}

PreProcessorGridAttributeCellDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

ColorMapSettingEditWidgetI* PreProcessorGridAttributeCellDataItem::PropertyDialog::widget() const
{
	return dynamic_cast<ColorMapSettingEditWidgetI*> (ui->widgetContainer->widget());
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	ui->widgetContainer->setWidget(w);
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);
}

OpacityContainer PreProcessorGridAttributeCellDataItem::PropertyDialog::opacity() const
{
	return ui->transparencyWidget->opacity();
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::setOpacity(const OpacityContainer& opacity)
{
	return ui->transparencyWidget->setOpacity(opacity);
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::accept()
{
	m_item->pushOpacityPercentAndUpdateActorSettingCommand(opacity(), widget()->createModifyCommand(), false);
	QDialog::accept();
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::apply()
{
	m_item->pushOpacityPercentAndUpdateActorSettingCommand(opacity(), widget()->createModifyCommand(), true);
	m_applied = true;
}
