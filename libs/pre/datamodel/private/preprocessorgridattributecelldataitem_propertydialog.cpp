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

int PreProcessorGridAttributeCellDataItem::PropertyDialog::opacityPercent() const
{
	return ui->transparencyWidget->opacityPercent();
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacityPercent(opacity);
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::accept()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::accept();
}

void PreProcessorGridAttributeCellDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_item->renderGraphicsView();
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
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	m_item->setOpacityPercentAndUpdateActorSettings(opacityPercent(), widget()->createModifyCommand(), true);
	m_applied = true;
}
