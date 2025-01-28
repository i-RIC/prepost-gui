#include "colormapsettingcontaineri.h"
#include "colormapsettingeditdialog.h"
#include "colormapsettingeditwidgeti.h"
#include "../project/projectdefaultcolormapsettings.h"
#include "ui_colormapsettingeditdialog.h"

#include <QVBoxLayout>

ColorMapSettingEditDialog::ColorMapSettingEditDialog(QWidget *parent) :
	QDialog(parent),
	m_widget {nullptr},
	m_targetName {},
	m_defaultColorMapSettings {nullptr},
	ui(new Ui::ColorMapSettingEditDialog)
{
	ui->setupUi(this);
	ui->setAsDefaultButton->hide();
	ui->setAsDefaultLabel->hide();

	connect(ui->setAsDefaultButton, &QPushButton::clicked, this, &ColorMapSettingEditDialog::setAsDefault);
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

void ColorMapSettingEditDialog::setupSetAsDefaultButton(const std::string& name, ProjectDefaultColorMapSettings* settings)
{
	m_targetName = name;
	m_defaultColorMapSettings = settings;

	ui->setAsDefaultButton->show();
	ui->setAsDefaultLabel->show();
}

QUndoCommand* ColorMapSettingEditDialog::createModifyCommand() const
{
	return m_widget->createModifyCommand();
}

void ColorMapSettingEditDialog::setAsDefault()
{
	m_defaultColorMapSettings->add(m_targetName, m_widget->setting()->copy());

	QMessageBox::information(this, tr("Information"), tr("Set as the default setting for this project."));
}

