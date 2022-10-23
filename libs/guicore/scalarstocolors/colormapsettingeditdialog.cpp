#include "colormapsettingcontaineri.h"
#include "colormapsettingeditdialog.h"
#include "colormapsettingeditwidgeti.h"
#include "ui_colormapsettingeditdialog.h"

#include <QVBoxLayout>

ColorMapSettingEditDialog::ColorMapSettingEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ColorMapSettingEditDialog),
	m_widget {nullptr}
{
	ui->setupUi(this);
}

ColorMapSettingEditDialog::~ColorMapSettingEditDialog()
{
	delete ui;
}

void ColorMapSettingEditDialog::disableApplyButton()
{
	ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
}

ColorMapSettingEditWidgetI* ColorMapSettingEditDialog::widget()
{
	return m_widget;
}

void ColorMapSettingEditDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	m_widget = w;
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);

	ui->widgetContainer->setWidget(w);
}

void ColorMapSettingEditDialog::setSetting(ColorMapSettingContainerI* setting)
{
	m_widget->setSetting(setting);
}

QUndoCommand* ColorMapSettingEditDialog::createModifyCommand() const
{
	return m_widget->createModifyCommand();
}
