#include "continuoussnapshotfilepropertypage.h"
#include "continuoussnapshotwizard.h"
#include "ui_continuoussnapshotfilepropertypage.h"

#include <QDir>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QSettings>

ContinuousSnapshotFilePropertyPage::ContinuousSnapshotFilePropertyPage(QWidget *parent) :
	QWizardPage(parent),
	ui(new Ui::ContinuousSnapshotFilePropertyPage)
{
	ui->setupUi(this);
	ui->formatComboBox->addItem(tr("PNG (*.png)"));
	m_extensionList.append(tr(".png"));
	ui->formatComboBox->addItem(tr("JPEG (*.jpg)"));
	m_extensionList.append(tr(".jpg"));
	ui->formatComboBox->addItem(tr("Windows Bitmap (*.bmp)"));
	m_extensionList.append(tr(".bmp"));
	ui->formatComboBox->addItem(tr("Portable Bitmap (*.ppm)"));
	m_extensionList.append(tr(".ppm"));
	ui->formatComboBox->addItem(tr("X11 Bitmap (*.xbm)"));
	m_extensionList.append(tr(".xbm"));

	m_wizard = dynamic_cast<ContinuousSnapshotWizard*>(parent);
}

ContinuousSnapshotFilePropertyPage::~ContinuousSnapshotFilePropertyPage()
{
	delete ui;
}

void ContinuousSnapshotFilePropertyPage::initializePage()
{
	// Directory
	ui->directoryEditWidget->setDirname(m_wizard->fileIODirectory());
	// Table view
	ui->prefixTableWidget->setColumnCount(1);
	ui->prefixTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Prefix")));
	switch (m_wizard->output()){
	case ContinuousSnapshotWizard::Onefile:
		ui->prefixTableWidget->setRowCount(1);
		ui->prefixTableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(tr("Output file")));
		ui->prefixTableWidget->setItem(0, 0, new QTableWidgetItem(tr("img_")));
		break;
	case ContinuousSnapshotWizard::Respectively:
		ui->prefixTableWidget->setRowCount(m_wizard->windowList().size());
		int idx = 0;
		for (QMdiSubWindow* sub : m_wizard->windowList()){
			ui->prefixTableWidget->setVerticalHeaderItem(idx, new QTableWidgetItem(sub->windowTitle()));
			ui->prefixTableWidget->setItem(idx, 0, new QTableWidgetItem(QString("window%1_").arg(idx + 1)));
			++idx;
		}
		break;
	}
	int size = ui->prefixTableWidget->rowCount();
	for (int i = 0; i < size; i++){
		ui->prefixTableWidget->setRowHeight(i, 20);
	}
	// Suffix length
	ui->suffixSpinBox->setValue(m_wizard->suffixLength());
	// Format
	QString ext = m_wizard->extension();
	if (ext == ".png"){
		ui->formatComboBox->setCurrentIndex(0);
	} else if (ext == ".jpg"){
		ui->formatComboBox->setCurrentIndex(1);
	} else if (ext == ".bmp"){
		ui->formatComboBox->setCurrentIndex(2);
	} else if (ext == ".ppm"){
		ui->formatComboBox->setCurrentIndex(3);
	} else if (ext == ".xbm"){
		ui->formatComboBox->setCurrentIndex(4);
	}
}

bool ContinuousSnapshotFilePropertyPage::validatePage()
{
	// Directory
	QDir dir(ui->directoryEditWidget->dirname());
	if (! dir.exists()){
		if (QMessageBox::Yes == QMessageBox::warning(this, tr("Warning"), tr("Folder %1 does not exists. Do you want to it now?").arg(QDir::toNativeSeparators(dir.absolutePath())), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)){
			bool ok = dir.mkpath(dir.absolutePath());
			if (! ok){
				QMessageBox::critical(this, tr("Error"), tr("Creating folder %1 failed.").arg(QDir::toNativeSeparators(dir.absolutePath())));
				return false;
			}
		} else {
			return false;
		}
	}
	if (ui->directoryEditWidget->dirname() == "") return false;
	m_wizard->setFileIODirectory(ui->directoryEditWidget->dirname());
	// Table view
	int size = ui->prefixTableWidget->rowCount();
	m_wizard->clearPrefixList();
	for (int i = 0; i < size; i++){
		QString prefix = ui->prefixTableWidget->item(i, 0)->text();
		m_wizard->addPrefixList(prefix);
	}
	// Suffix length
	m_wizard->setSuffixLength(ui->suffixSpinBox->value());
	// Format
	if (ui->formatComboBox->currentIndex() < 0) return false;
	QString ex = m_extensionList.at(ui->formatComboBox->currentIndex());
	m_wizard->setExtension(ex);

	return true;
}
