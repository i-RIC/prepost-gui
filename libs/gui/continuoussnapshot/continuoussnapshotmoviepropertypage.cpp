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
	// Output Movie
	ui->movieCheckBox->setChecked(m_wizard->outputMovie());
	// Table view
	ui->filenameTableWidget->setColumnCount(1);
	ui->filenameTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("File name")));
	switch (m_wizard->output()) {
	case ContinuousSnapshotWizard::Onefile:
		ui->filenameTableWidget->setRowCount(1);
		ui->filenameTableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(tr("Output file")));
		ui->filenameTableWidget->setItem(0, 0, new QTableWidgetItem("img.mp4"));
		break;
	case ContinuousSnapshotWizard::Respectively:
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
	if (m_wizard->movieLengthMode() == 0) {
		// specify length
		ui->lengthRadioButton->setChecked(true);
		ui->lengthSpinBox->setValue(m_wizard->movieLength());
	} else {
		// specify FPS
		ui->fpsRadioButton->setChecked(true);
		ui->fpsSpinBox->setValue(m_wizard->framesPerSecond());
	}
	// Profiles
	ui->profileComboBox->addItem(tr("Default"));
	ui->profileComboBox->setCurrentIndex(m_wizard->movieProfile());
}

bool ContinuousSnapshotMoviePropertyPage::validatePage()
{
	// Output Movie
	m_wizard->setOutputMovie(ui->movieCheckBox->isChecked());
	if (ui->lengthRadioButton->isChecked()) {
		// Length
		m_wizard->setMovieLengthMode(0);
		m_wizard->setMovieLength(ui->lengthSpinBox->value());
	} else {
		// FPS
		m_wizard->setMovieLengthMode(1);
		m_wizard->setFramesPerSecond(ui->fpsSpinBox->value());
	}
	// Profile
	m_wizard->setMovieProfile(ui->profileComboBox->currentIndex());
	return true;
}

QStringList ContinuousSnapshotMoviePropertyPage::getProfile(int profileid)
{
	QStringList ret;
	switch (profileid) {
	case 0:
	default:
		ret << "-qscale" << "0" << "-vcodec" << "libx264" << "-pix_fmt" << "yuv420p";
		break;
	}
	return ret;
}
