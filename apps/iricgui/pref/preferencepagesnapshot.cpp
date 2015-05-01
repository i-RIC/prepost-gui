#include "ui_preferencepagesnapshot.h"

#include "preferencepagesnapshot.h"

#include <QSettings>

PreferencePageSnapshot::PreferencePageSnapshot(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageSnapshot)
{
	ui->setupUi(this);
	setupTypeComboBox();
	m_settings.beginGroup("snapshot");

	QString prefix = m_settings.value("prefix", "img_").value<QString>();
	ui->prefixEdit->setText(prefix);

	int suffix = m_settings.value("suffixlen", 4).value<int>();
	ui->suffixSpinBox->setValue(suffix);

	QString ext = m_settings.value("type", "png").value<QString>();
	int index = m_exts.indexOf(ext);
	if (index != -1) {ui->typeComboBox->setCurrentIndex(index);}
}

PreferencePageSnapshot::~PreferencePageSnapshot()
{
	delete ui;
}

void PreferencePageSnapshot::update()
{
	m_settings.setValue("prefix", ui->prefixEdit->text());
	m_settings.setValue("suffixlen", ui->suffixSpinBox->value());
	int index = ui->typeComboBox->currentIndex();
	m_settings.setValue("type", m_exts.at(index));
}

void PreferencePageSnapshot::setupTypeComboBox()
{
	ui->typeComboBox->addItem(tr("PNG (*.png)"));
	m_exts.append("png");

	ui->typeComboBox->addItem(tr("JPEG (*.jpg)"));
	m_exts.append("jpg");

	ui->typeComboBox->addItem(tr("Windows Bitmap (*.bmp)"));
	m_exts.append("bmp");

	ui->typeComboBox->addItem(tr("Portable Bitmap (*.ppm)"));
	m_exts.append("ppm");

	ui->typeComboBox->addItem(tr("X11 Bitmap (*.xbm)"));
	m_exts.append("xbm");
}
