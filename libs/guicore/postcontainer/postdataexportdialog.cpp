#include "ui_postdataexportdialog.h"

#include "postdataexportdialog.h"

#include <QFileDialog>
#include <QMessageBox>

PostDataExportDialog::PostDataExportDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PostDataExportDialog}
{
	ui->setupUi(this);
	ui->fileEdit->setSaveMode(true);

	ui->fileLabel->hide();
	ui->fileEdit->hide();

	connect(ui->dataAllCheckBox, SIGNAL(toggled(bool)), this, SLOT(allCheckChange(bool)));
	connect(ui->dataStartSlider, SIGNAL(valueChanged(int)), this, SLOT(handleStartChange(int)));
	connect(ui->dataEndSlider, SIGNAL(valueChanged(int)), this, SLOT(handleEndChange(int)));
	connect(ui->fullRegionCheckBox, SIGNAL(toggled(bool)), this, SLOT(fullRegionChange(bool)));
	connect(ui->iminSlider, SIGNAL(valueChanged(int)), this, SLOT(handleIMinChange(int)));
	connect(ui->imaxSlider, SIGNAL(valueChanged(int)), this, SLOT(handleIMaxChange(int)));
	connect(ui->jminSlider, SIGNAL(valueChanged(int)), this, SLOT(handleJMinChange(int)));
	connect(ui->jmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(handleJMaxChange(int)));
	connect(ui->kminSlider, SIGNAL(valueChanged(int)), this, SLOT(handleKMinChange(int)));
	connect(ui->kmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(handleKMaxChange(int)));
	connect(ui->folderRefButton, SIGNAL(clicked()), this, SLOT(handleRefButtonClick()));
	connect(ui->detailButton, SIGNAL(clicked()), this, SLOT(toggleRangeGroupBox()));

	ui->dataRangeGroupBox->hide();
	adjustSize();
}

PostDataExportDialog::~PostDataExportDialog()
{
	delete ui;
}

void PostDataExportDialog::setFileMode()
{
	ui->folderLabel->hide();
	ui->folderEdit->hide();
	ui->folderRefButton->hide();
	ui->prefixLabel->hide();
	ui->prefixEdit->hide();

	ui->fileLabel->show();
	ui->fileEdit->show();
	updateGeometry();
}

void PostDataExportDialog::setTimeValues(QList<double> timevalues)
{
	ui->dataStartSlider->setValues(timevalues);
	ui->dataEndSlider->setValues(timevalues);
}

void PostDataExportDialog::setIJKRange(int inum, int jnum, int knum)
{
	ui->iminSlider->setMinimum(1);
	ui->iminSlider->setMaximum(inum);
	ui->imaxSlider->setMinimum(1);
	ui->imaxSlider->setMaximum(inum);
	if (jnum == 1) {
		ui->jminLabel->hide();
		ui->jminSlider->hide();
		ui->jmaxLabel->hide();
		ui->jmaxSlider->hide();
	}
	ui->jminSlider->setMinimum(1);
	ui->jmaxSlider->setMinimum(1);
	ui->jminSlider->setMaximum(jnum);
	ui->jmaxSlider->setMaximum(jnum);
	if (knum == 1) {
		ui->kminLabel->hide();
		ui->kminSlider->hide();
		ui->kmaxLabel->hide();
		ui->kmaxSlider->hide();
	}
	ui->kminSlider->setMinimum(1);
	ui->kmaxSlider->setMinimum(1);
	ui->kminSlider->setMaximum(knum);
	ui->kmaxSlider->setMaximum(knum);
}

void PostDataExportDialog::hideFormat()
{
	ui->formatLabel->hide();
	ui->formatComboBox->hide();
	updateGeometry();
}

void PostDataExportDialog::hideDataRange()
{
	ui->dataRangeGroupBox->hide();
	ui->detailButton->hide();
	updateGeometry();
}

void PostDataExportDialog::setFormat(Format f)
{
	if (f == Format::VTKASCII) {
		ui->formatComboBox->setCurrentIndex(0);
	} else if (f == Format::VTKBinary) {
		ui->formatComboBox->setCurrentIndex(1);
	} else if (f == Format::CSV) {
		ui->formatComboBox->setCurrentIndex(2);
	} else if (f == Format::TPO) {
		ui->formatComboBox->setCurrentIndex(3);
	} else if (f == Format::ESRIShape) {
		ui->formatComboBox->setCurrentIndex(4);
	}
}

void PostDataExportDialog::setExportSetting(const PostExportSetting &setting)
{
	m_setting = setting;
	ui->folderEdit->setText(QDir::toNativeSeparators(setting.folder));
	ui->fileEdit->setFilename(setting.filename);
	ui->dataAllCheckBox->setChecked(setting.allSteps);
	if (setting.allSteps) {
		ui->dataStartSlider->setValue(ui->dataStartSlider->minimum());
		ui->dataEndSlider->setValue(ui->dataEndSlider->maximum());
	} else {
		ui->dataStartSlider->setValue(setting.startStep);
		ui->dataEndSlider->setValue(setting.endStep);
	}
	ui->samplingSpinBox->setValue(setting.skipRate);

	ui->fullRegionCheckBox->setChecked(setting.fullRange);
	if (setting.fullRange){
		ui->iminSlider->setValue(ui->iminSlider->minimum());
		ui->imaxSlider->setValue(ui->imaxSlider->maximum());
		ui->jminSlider->setValue(ui->jminSlider->minimum());
		ui->jmaxSlider->setValue(ui->jmaxSlider->maximum());
		ui->kminSlider->setValue(ui->kminSlider->minimum());
		ui->kmaxSlider->setValue(ui->kmaxSlider->maximum());
	} else {
		ui->iminSlider->setValue(setting.iMin + 1);
		ui->imaxSlider->setValue(setting.iMax + 1);
		ui->jminSlider->setValue(setting.jMin + 1);
		ui->jmaxSlider->setValue(setting.jMax + 1);
		ui->kminSlider->setValue(setting.kMin + 1);
		ui->kmaxSlider->setValue(setting.kMax + 1);

		ui->dataRangeGroupBox->show();
		adjustSize();
	}
	ui->prefixEdit->setText(setting.prefix);
}

void PostDataExportDialog::setPrefix(const QString& prefix)
{
	ui->prefixEdit->setText(prefix);
}

PostDataExportDialog::Format PostDataExportDialog::format() const
{
	switch (ui->formatComboBox->currentIndex()) {
	case 0:
		return Format::VTKASCII;
		break;
	case 1:
		return Format::VTKBinary;
		break;
	case 2:
		return Format::CSV;
		break;
	case 3:
		return Format::TPO;
		break;
	case 4:
		return Format::ESRIShape;
		break;
	default:
		return Format::VTKASCII;
	}
}

PostExportSetting PostDataExportDialog::exportSetting() const
{
	PostExportSetting ret = m_setting;
	ret.folder = ui->folderEdit->text();
	ret.filename = ui->fileEdit->filename();

	ret.allSteps = ui->dataAllCheckBox->isChecked();
	ret.startStep = ui->dataStartSlider->value();
	ret.endStep = ui->dataEndSlider->value();
	ret.skipRate = ui->samplingSpinBox->value();

	ret.fullRange = ui->fullRegionCheckBox->isChecked();
	ret.iMin = ui->iminSlider->value() - 1;
	ret.iMax = ui->imaxSlider->value() - 1;
	ret.jMin = ui->jminSlider->value() - 1;
	ret.jMax = ui->jmaxSlider->value() - 1;
	ret.kMin = ui->kminSlider->value() - 1;
	ret.kMax = ui->kmaxSlider->value() - 1;

	ret.prefix = ui->prefixEdit->text();

	return ret;
}

QString PostDataExportDialog::prefix() const
{
	return ui->prefixEdit->text();
}

void PostDataExportDialog::allCheckChange(bool checked)
{
	ui->dataStartSlider->setDisabled(checked);
	ui->dataEndSlider->setDisabled(checked);
	ui->samplingSpinBox->setDisabled(checked);

	if (checked) {
		ui->dataStartSlider->setValue(ui->dataStartSlider->minimum());
		ui->dataEndSlider->setValue(ui->dataEndSlider->maximum());
		ui->samplingSpinBox->setValue(1);
	}
}

void PostDataExportDialog::handleStartChange(int val)
{
	if (val > ui->dataEndSlider->value()) {
		ui->dataEndSlider->setValue(val);
	}
	updateSkipRateMaximum();
}

void PostDataExportDialog::handleEndChange(int val)
{
	if (val < ui->dataStartSlider->value()) {
		ui->dataStartSlider->setValue(val);
	}
	updateSkipRateMaximum();
}

void PostDataExportDialog::fullRegionChange(bool checked)
{
	ui->iminSlider->setDisabled(checked);
	ui->imaxSlider->setDisabled(checked);
	ui->jminSlider->setDisabled(checked);
	ui->jmaxSlider->setDisabled(checked);
	ui->kminSlider->setDisabled(checked);
	ui->kmaxSlider->setDisabled(checked);

	if (checked) {
		ui->iminSlider->setValue(ui->iminSlider->minimum());
		ui->imaxSlider->setValue(ui->imaxSlider->maximum());
		ui->jminSlider->setValue(ui->jminSlider->minimum());
		ui->jmaxSlider->setValue(ui->jmaxSlider->maximum());
		ui->kminSlider->setValue(ui->kminSlider->minimum());
		ui->kmaxSlider->setValue(ui->kmaxSlider->maximum());
	}
}

void PostDataExportDialog::handleIMinChange(int val)
{
	if (val > ui->imaxSlider->value()) {
		ui->imaxSlider->setValue(val);
	}
}

void PostDataExportDialog::handleIMaxChange(int val)
{
	if (val < ui->iminSlider->value()) {
		ui->iminSlider->setValue(val);
	}
}

void PostDataExportDialog::handleJMinChange(int val)
{
	if (val > ui->jmaxSlider->value()) {
		ui->jmaxSlider->setValue(val);
	}
}

void PostDataExportDialog::handleJMaxChange(int val)
{
	if (val < ui->jminSlider->value()) {
		ui->jminSlider->setValue(val);
	}
}

void PostDataExportDialog::handleKMinChange(int val)
{
	if (val > ui->kmaxSlider->value()) {
		ui->kmaxSlider->setValue(val);
	}
}

void PostDataExportDialog::handleKMaxChange(int val)
{
	if (val < ui->kminSlider->value()) {
		ui->kminSlider->setValue(val);
	}
}

void PostDataExportDialog::handleRefButtonClick()
{
	QString oldDir = QDir::fromNativeSeparators(ui->folderEdit->text());
	QString newDir = QFileDialog::getExistingDirectory(this, tr("Select Folder"), oldDir);
	if (newDir.isNull()) {return;}
	ui->folderEdit->setText(QDir::toNativeSeparators(newDir));
}

void PostDataExportDialog::accept()
{
	if (ui->folderEdit->isVisible()) {
		QString folderName = QDir::fromNativeSeparators(ui->folderEdit->text());
		if (! QFile::exists(folderName)) {
			QMessageBox::warning(this, tr("Warning"), tr("Folder %1 does not exists.").arg(ui->folderEdit->text()));
			return;
		}
	}
	QDialog::accept();
}


void PostDataExportDialog::toggleRangeGroupBox()
{
	bool visible = ! ui->dataRangeGroupBox->isVisible();
	if (visible) {
		ui->detailButton->setText(tr("&Hide Detail"));
	} else {
		ui->detailButton->setText(tr("Show &Detail"));
	}
	ui->dataRangeGroupBox->setVisible(visible);
	adjustSize();
}

void PostDataExportDialog::updateSkipRateMaximum()
{
	int max = ui->dataEndSlider->value() - ui->dataStartSlider->value();
	if (max == 0) {max = 1;}
	ui->samplingSpinBox->setMaximum(max);
}
