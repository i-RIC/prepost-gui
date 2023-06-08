#include "continuoussnapshotwizard_filepropertypage.h"
#include "ui_continuoussnapshotwizard_filepropertypage.h"

#include <unordered_set>

namespace {

QString newPrefix(std::unordered_set<QString>* usedPrefixes)
{
	int index = 1;

	while (true) {
		auto prefix = QString("window%1_").arg(index);
		auto it = usedPrefixes->find(prefix);

		if (it == usedPrefixes->end()) {
			usedPrefixes->insert(prefix);
			return prefix;
		}

		++ index;
	}
}

} // namespace

ContinuousSnapshotWizard::FilePropertyPage::FilePropertyPage(ContinuousSnapshotWizard *wizard) :
	QWizardPage(wizard),
	m_wizard {wizard},
	ui(new Ui::ContinuousSnapshotWizard_FilePropertyPage)
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
}

ContinuousSnapshotWizard::FilePropertyPage::~FilePropertyPage()
{
	delete ui;
}

void ContinuousSnapshotWizard::FilePropertyPage::initializePage()
{
	auto setting = m_wizard->setting();
	auto& windowSettings = setting.windowSettings;

	std::unordered_set<QString> usedPrefixes;
	for (const auto& pair : windowSettings) {
		if (pair.second.prefix != "") {
			usedPrefixes.insert(pair.second.prefix);
		}
	}

	// Directory
	ui->directoryEditWidget->setDirname(setting.exportTargetFolder);
	// Table view
	ui->prefixTableWidget->setColumnCount(1);
	ui->prefixTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Prefix")));
	switch (setting.fileOutputSetting) {
	case ContinuousSnapshotSetting::FileOutputSetting::Onefile:
		ui->prefixTableWidget->setRowCount(1);
		ui->prefixTableWidget->setVerticalHeaderItem(0, new QTableWidgetItem(tr("Output file")));
		ui->prefixTableWidget->setItem(0, 0, new QTableWidgetItem(tr("img_")));
		break;
	case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
		ui->prefixTableWidget->setRowCount(m_wizard->windowList().size());
		int idx = 0;
		for (auto sub : m_wizard->windowList()) {
			auto it = windowSettings.find(sub->windowTitle());
			if (it->second.prefix == "") {
				it->second.prefix = newPrefix(&usedPrefixes);
			}

			ui->prefixTableWidget->setVerticalHeaderItem(idx, new QTableWidgetItem(sub->windowTitle()));
			ui->prefixTableWidget->setItem(idx, 0, new QTableWidgetItem(it->second.prefix));
			++idx;
		}
		break;
	}
	int size = ui->prefixTableWidget->rowCount();
	for (int i = 0; i < size; i++) {
		ui->prefixTableWidget->setRowHeight(i, 20);
	}
	// Suffix length
	ui->suffixSpinBox->setValue(setting.suffixLength);
	// Format
	auto ext = setting.imageExtention;
	if (ext == ".png") {
		ui->formatComboBox->setCurrentIndex(0);
	} else if (ext == ".jpg") {
		ui->formatComboBox->setCurrentIndex(1);
	} else if (ext == ".bmp") {
		ui->formatComboBox->setCurrentIndex(2);
	} else if (ext == ".ppm") {
		ui->formatComboBox->setCurrentIndex(3);
	} else if (ext == ".xbm") {
		ui->formatComboBox->setCurrentIndex(4);
	}
}
bool ContinuousSnapshotWizard::FilePropertyPage::validatePage()
{
	auto setting = m_wizard->setting();
	auto& windowSettings = setting.windowSettings;

	const auto& wl = m_wizard->windowList();

	// Directory
	QDir dir(ui->directoryEditWidget->dirname());
	if (! dir.exists()) {
		if (QMessageBox::Yes == QMessageBox::warning(this, tr("Warning"), tr("Folder %1 does not exists. Do you want to it now?").arg(QDir::toNativeSeparators(dir.absolutePath())), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
			bool ok = dir.mkpath(dir.absolutePath());
			if (! ok) {
				QMessageBox::critical(this, tr("Error"), tr("Creating folder %1 failed.").arg(QDir::toNativeSeparators(dir.absolutePath())));
				return false;
			}
		} else {
			return false;
		}
	}
	if (ui->directoryEditWidget->dirname() == "") {return false;}

	setting.exportTargetFolder = ui->directoryEditWidget->dirname();

	// Table view
	int size = ui->prefixTableWidget->rowCount();
	m_wizard->clearPrefixList();
	for (int i = 0; i < size; i++) {
		auto prefix = ui->prefixTableWidget->item(i, 0)->text();
		m_wizard->addPrefixList(prefix);

		if (setting.fileOutputSetting == ContinuousSnapshotSetting::FileOutputSetting::Respectively) {
			auto w = wl.at(i);
			auto title = w->windowTitle();
			auto it = windowSettings.find(title);
			it->second.prefix = prefix;
		}
	}
	// Suffix length
	setting.suffixLength = ui->suffixSpinBox->value();

	// Format
	if (ui->formatComboBox->currentIndex() < 0) {return false;}
	setting.imageExtention = m_extensionList.at(ui->formatComboBox->currentIndex());

	m_wizard->setSetting(setting);

	return true;
}
