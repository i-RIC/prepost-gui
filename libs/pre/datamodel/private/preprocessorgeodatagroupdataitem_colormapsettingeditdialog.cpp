#include "../preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem_colormapsettingeditdialog.h"
#include "ui_preprocessorgeodatagroupdataitem_colormapsettingeditdialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>

PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::ColorMapSettingEditDialog(PreProcessorGeoDataGroupDataItem* item, QWidget *parent) :
	QDialog {parent},
	m_applied {false},
	m_item {item},
	m_widget {nullptr},
	ui(new Ui::PreProcessorGeoDataGroupDataItem_ColorMapSettingEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_gridTypeDataItem = dynamic_cast<PreProcessorGridTypeDataItem*> (item->parent()->parent());

	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &ColorMapSettingEditDialog::handleButtonClick);
}

PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::~ColorMapSettingEditDialog()
{
	delete ui;
}

ColorMapSettingEditWidgetI* PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::widget()
{
	return m_widget;
}

void PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	m_widget = w;
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);

	ui->widgetContainer->setWidget(w);
}

void PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::setSetting(ColorMapSettingContainerI* setting)
{
	m_widget->setSetting(setting);
}

QUndoCommand* PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::createModifyCommand() const
{
	return m_widget->createModifyCommand();
}

void PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::accept()
{
	m_item->pushCommand(m_gridTypeDataItem->createApplyColorMapSettingCommand(m_item->condition()->name(), createModifyCommand(), false));

	QDialog::accept();
}

void PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_item->renderGraphicsView();
	}

	QDialog::reject();
}

void PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog::apply()
{
	m_item->pushCommand(m_gridTypeDataItem->createApplyColorMapSettingCommand(m_item->condition()->name(), createModifyCommand(), true));
	m_applied = true;
}
