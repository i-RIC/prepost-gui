#include "../preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridattributenodedataitem_propertydialog.h"
#include "ui_preprocessorgridattributenodedataitem_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

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

OpacityContainer PreProcessorGridAttributeNodeDataItem::PropertyDialog::opacity() const
{
	return ui->transparencyWidget->opacity();
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::setOpacity(const OpacityContainer& opacity)
{
	return ui->transparencyWidget->setOpacity(opacity);
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::accept()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(false), m_item);
	QDialog::accept();
}

void PreProcessorGridAttributeNodeDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
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
	m_item->pushUpdateActorSettingCommand(createModifyCommand(true), m_item);
	m_applied = true;
}

QUndoCommand* PreProcessorGridAttributeNodeDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto ret = new MergeSupportedListCommand(iRIC::generateCommandId("PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog"), apply);
	ret->addCommand(widget()->createModifyCommand());
	ret->addCommand(new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("Opacity"), apply, opacity(), &m_item->opacity()));

	return ret;
}
