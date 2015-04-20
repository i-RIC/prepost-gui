#include "postdataexportdialog.h"
#include "ui_postdataexportdialog.h"
#include <QFileDialog>
#include <QMessageBox>

PostDataExportDialog::PostDataExportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostDataExportDialog)
{
	ui->setupUi(this);

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
	if (jnum == 1){
		ui->jminLabel->hide();
		ui->jminSlider->hide();
		ui->jmaxLabel->hide();
		ui->jmaxSlider->hide();
	}
	ui->jminSlider->setMinimum(1);
	ui->jmaxSlider->setMinimum(1);
	ui->jminSlider->setMaximum(jnum);
	ui->jmaxSlider->setMaximum(jnum);
	if (knum == 1){
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
	if (f == fmVTK){
		ui->formatComboBox->setCurrentIndex(0);
	} else if (f == fmCSV){
		ui->formatComboBox->setCurrentIndex(1);
	}
}

void PostDataExportDialog::setOutputFolder(const QString& folder)
{
	ui->folderEdit->setText(QDir::toNativeSeparators(folder));
}

void PostDataExportDialog::setOutputFileName(const QString& filename)
{
	ui->fileEdit->setFilename(filename);
}

void PostDataExportDialog::setAllTimeSteps(bool all)
{
	ui->dataAllCheckBox->setChecked(all);
	if (all){
		ui->dataStartSlider->setValue(ui->dataStartSlider->minimum());
		ui->dataEndSlider->setValue(ui->dataEndSlider->maximum());
	}
}

void PostDataExportDialog::setStartTimeStep(int start)
{
	if (ui->dataAllCheckBox->isChecked()){return;}
	ui->dataStartSlider->setValue(start);
}

void PostDataExportDialog::setEndTimeStep(int end)
{
	if (ui->dataAllCheckBox->isChecked()){return;}
	ui->dataEndSlider->setValue(end);
}

void PostDataExportDialog::setFullRange(bool full)
{
	ui->fullRegionCheckBox->setChecked(full);
	if (full){
		ui->iminSlider->setValue(ui->iminSlider->minimum());
		ui->imaxSlider->setValue(ui->imaxSlider->maximum());
		ui->jminSlider->setValue(ui->jminSlider->minimum());
		ui->jmaxSlider->setValue(ui->jmaxSlider->maximum());
		ui->kminSlider->setValue(ui->kminSlider->minimum());
		ui->kmaxSlider->setValue(ui->kmaxSlider->maximum());
	} else {
		ui->dataRangeGroupBox->show();
		adjustSize();
	}
}

void PostDataExportDialog::setIMin(int imin)
{
	if (ui->fullRegionCheckBox->isChecked()){return;}
	ui->iminSlider->setValue(imin + 1);
}

void PostDataExportDialog::setIMax(int imax)
{
	if (ui->fullRegionCheckBox->isChecked()){return;}
	ui->imaxSlider->setValue(imax + 1);
}

void PostDataExportDialog::setJMin(int jmin)
{
	if (ui->fullRegionCheckBox->isChecked()){return;}
	ui->jminSlider->setValue(jmin + 1);
}

void PostDataExportDialog::setJMax(int jmax)
{
	if (ui->fullRegionCheckBox->isChecked()){return;}
	ui->jmaxSlider->setValue(jmax + 1);
}

void PostDataExportDialog::setKMin(int kmin)
{
	if (ui->fullRegionCheckBox->isChecked()){return;}
	ui->kminSlider->setValue(kmin + 1);
}

void PostDataExportDialog::setKMax(int kmax)
{
	if (ui->fullRegionCheckBox->isChecked()){return;}
	ui->kmaxSlider->setValue(kmax + 1);
}

void PostDataExportDialog::setPrefix(const QString& prefix)
{
	ui->prefixEdit->setText(prefix);
}

void PostDataExportDialog::setSkipRate(int rate)
{
	ui->samplingSpinBox->setValue(rate);
}

PostDataExportDialog::Format PostDataExportDialog::format()
{
	if (ui->formatComboBox->currentIndex() == 0){
		return fmVTK;
	} else {
		return fmCSV;
	}
}

const QString PostDataExportDialog::outputFolder() const
{
	return ui->folderEdit->text();
}

const QString PostDataExportDialog::outputFileName() const
{
	return ui->fileEdit->filename();
}

bool PostDataExportDialog::allTimeSteps()
{
	return ui->dataAllCheckBox->isChecked();
}


int PostDataExportDialog::startTimeStep()
{
	return ui->dataStartSlider->value();
}

int PostDataExportDialog::endTimeStep()
{
	return ui->dataEndSlider->value();
}

bool PostDataExportDialog::fullRange()
{
	return ui->fullRegionCheckBox->isChecked();
}

int PostDataExportDialog::iMin()
{
	return ui->iminSlider->value() - 1;
}

int PostDataExportDialog::iMax()
{
	return ui->imaxSlider->value() - 1;
}

int PostDataExportDialog::jMin()
{
	return ui->jminSlider->value() - 1;
}

int PostDataExportDialog::jMax()
{
	return ui->jmaxSlider->value() - 1;
}

int PostDataExportDialog::kMin()
{
	return ui->kminSlider->value() - 1;
}

int PostDataExportDialog::kMax()
{
	return ui->kmaxSlider->value() - 1;
}

const QString PostDataExportDialog::prefix()
{
	return ui->prefixEdit->text();
}

int PostDataExportDialog::skipRate()
{
	return ui->samplingSpinBox->value();
}

void PostDataExportDialog::allCheckChange(bool checked)
{
	ui->dataStartSlider->setDisabled(checked);
	ui->dataEndSlider->setDisabled(checked);
	ui->samplingSpinBox->setDisabled(checked);

	if (checked){
		ui->dataStartSlider->setValue(ui->dataStartSlider->minimum());
		ui->dataEndSlider->setValue(ui->dataEndSlider->maximum());
		ui->samplingSpinBox->setValue(1);
	}
}

void PostDataExportDialog::handleStartChange(int val)
{
	if (val > ui->dataEndSlider->value()){
		ui->dataEndSlider->setValue(val);
	}
	updateSkipRateMaximum();
}

void PostDataExportDialog::handleEndChange(int val)
{
	if (val < ui->dataStartSlider->value()){
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

	if (checked){
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
	if (val > ui->imaxSlider->value()){
		ui->imaxSlider->setValue(val);
	}
}

void PostDataExportDialog::handleIMaxChange(int val)
{
	if (val < ui->iminSlider->value()){
		ui->iminSlider->setValue(val);
	}
}

void PostDataExportDialog::handleJMinChange(int val)
{
	if (val > ui->jmaxSlider->value()){
		ui->jmaxSlider->setValue(val);
	}
}

void PostDataExportDialog::handleJMaxChange(int val)
{
	if (val < ui->jminSlider->value()){
		ui->jminSlider->setValue(val);
	}
}

void PostDataExportDialog::handleKMinChange(int val)
{
	if (val > ui->kmaxSlider->value()){
		ui->kmaxSlider->setValue(val);
	}
}

void PostDataExportDialog::handleKMaxChange(int val)
{
	if (val < ui->kminSlider->value()){
		ui->kminSlider->setValue(val);
	}
}

void PostDataExportDialog::handleRefButtonClick()
{
	QString oldDir = QDir::fromNativeSeparators(ui->folderEdit->text());
	QString newDir = QFileDialog::getExistingDirectory(this, tr("Select Folder"), oldDir);
	if (newDir.isNull()){return;}
	ui->folderEdit->setText(QDir::toNativeSeparators(newDir));
}

void PostDataExportDialog::accept()
{
	if (ui->folderEdit->isVisible()){
		QString folderName = QDir::fromNativeSeparators(ui->folderEdit->text());
		if (! QFile::exists(folderName)){
			QMessageBox::warning(this, tr("Warning"), tr("Folder %1 does not exists.").arg(ui->folderEdit->text()));
			return;
		}
	}
	QDialog::accept();
}


void PostDataExportDialog::toggleRangeGroupBox()
{
	bool visible = ! ui->dataRangeGroupBox->isVisible();
	if (visible){
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
	if (max == 0){max = 1;}
	ui->samplingSpinBox->setMaximum(max);
}
