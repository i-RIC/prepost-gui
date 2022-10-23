#include "../preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridattributenodedataitem_propertydialog.h"
#include "ui_preprocessorgridattributenodedataitem_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>

PreProcessorGridAttributeNodeDataItem::PropertyDialog::PropertyDialog(PreProcessorGridAttributeNodeGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::PreProcessorGridAttributeNodeDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
}

PreProcessorGridAttributeNodeDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

ColorMapSettingEditWidgetI* PreProcessorGridAttributeNodeDataItem::PropertyDialog::widget() const
{
	return dynamic_cast<ColorMapSettingEditWidgetI*> (ui->widgetContainer->widget());
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	ui->widgetContainer->setWidget(w);
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);
}

int PreProcessorGridAttributeNodeDataItem::PropertyDialog::opacityPercent() const
{
	return ui->transparencyWidget->opacityPercent();
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacityPercent(opacity);
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::accept()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::accept();
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_item->renderGraphicsView();
	}

	QDialog::reject();
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::apply()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	m_item->setOpacityPercentAndUpdateActorSettings(opacityPercent(), widget()->createModifyCommand(), true);
	m_applied = true;
}
