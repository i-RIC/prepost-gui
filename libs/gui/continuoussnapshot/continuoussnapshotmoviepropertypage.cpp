#include "ui_continuoussnapshotmoviepropertypage.h"

#include "continuoussnapshotmoviepropertypage.h"
#include "continuoussnapshotwizard.h"

#include <QDir>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSettings>

ContinuousSnapshotMoviePropertyPage::ContinuousSnapshotMoviePropertyPage(QWidget* parent) :
	QWizardPage(parent),
	ui(new Ui::ContinuousSnapshotMoviePropertyPage)
{
	ui->setupUi(this);

	m_wizard = dynamic_cast<ContinuousSnapshotWizard*>(parent);
}

ContinuousSnapshotMoviePropertyPage::~ContinuousSnapshotMoviePropertyPage()
{
	delete ui;
}

void ContinuousSnapshotMoviePropertyPage::initializePage()
{
	const auto s = m_wizard->setting();

	// Output Movie
	ui->movieCheckBox->setChecked(s.outputMovie);
	// Table view
	ui->filenameTableWidget->setColumnCount(1);
	ui->filenameTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("File name")));
	switch (s.fileOutputSetting) {
	case ContinuousSnapshotSetting::FileOutputSetting::Onefile:
		ui->filenameTableWidget->setRowCount(1);
		ui->filenameTableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(tr("Output file")));
		ui->filenameTableWidget->setItem(0, 0, new QTableWidgetItem("img.mp4"));
		break;
	case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
		ui->filenameTableWidget->setRowCount(m_wizard->windowList().size());
		int idx = 0;
		for (QMdiSubWindow* sub : m_wizard->windowList()) {
			ui->filenameTableWidget->setVerticalHeaderItem(idx, new QTableWidgetItem(sub->windowTitle()));
			ui->filenameTableWidget->setItem(idx, 0, new QTableWidgetItem(QString("window%1.mp4").arg(idx + 1)));
			++idx;
		}
		break;
	}
	int size = ui->filenameTableWidget->rowCount();
	for (int i = 0; i < size; i++) {
		ui->filenameTableWidget->setRowHeight(i, 20);
	}
	if (s.movieLengthMode == ContinuousSnapshotSetting::MovieLengthMode::Length) {
		// specify length
		ui->lengthRadioButton->setChecked(true);
		ui->lengthSpinBox->setValue(s.movieLengthSeconds);
	} else {
		// specify FPS
		ui->fpsRadioButton->setChecked(true);
		ui->fpsSpinBox->setValue(s.movieFramesPerSeconds);
	}
	// Profiles
	ui->profileComboBox->addItem(tr("Default"));
	ui->profileComboBox->setCurrentIndex(static_cast<int> (s.movieProfile));
}

bool ContinuousSnapshotMoviePropertyPage::validatePage()
{
	auto s = m_wizard->setting();

	// Output Movie
	s.outputMovie = ui->movieCheckBox->isChecked();
	if (ui->lengthRadioButton->isChecked()) {
		// Length
		s.movieLengthMode = ContinuousSnapshotSetting::MovieLengthMode::Length;
		s.movieLengthSeconds = ui->lengthSpinBox->value();
	} else {
		// FPS
		s.movieLengthMode = ContinuousSnapshotSetting::MovieLengthMode::FPS;
		s.movieFramesPerSeconds = ui->fpsSpinBox->value();
	}
	// Profile
	s.movieProfile = static_cast<ContinuousSnapshotSetting::MovieProfile> (ui->profileComboBox->currentIndex());

	m_wizard->setSetting(s);

	return true;
}

QStringList ContinuousSnapshotMoviePropertyPage::getProfile(ContinuousSnapshotSetting::MovieProfile profile)
{
	QStringList ret;
	switch (profile) {
	case ContinuousSnapshotSetting::MovieProfile::MP4:
	default:
		ret << "-qscale" << "0" << "-vcodec" << "libx264" << "-pix_fmt" << "yuv420p";
		break;
	}
	return ret;
}
