#include "continuoussnapshotwizard_confirmpage.h"

#include <QDir>
#include <QVBoxLayout>

ContinuousSnapshotWizard::ConfirmPage::ConfirmPage(ContinuousSnapshotWizard *wizard) :
	QWizardPage(wizard),
	m_wizard {wizard},
	m_fileList {new QListWidget(this)}
{
	setTitle(tr("Confirm the result"));
	setSubTitle(tr("The files in the list below will be created or updated. "));

	m_fileList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_fileList->setSelectionMode(QAbstractItemView::NoSelection);

	auto layout = new QVBoxLayout();
	layout->addWidget(m_fileList);
	setLayout(layout);
}

void ContinuousSnapshotWizard::ConfirmPage::initializePage()
{
	const auto s = m_wizard->setting();

	m_fileList->clear();
	m_wizard->clearFileList();

	int len = s.suffixLength;
	int step = s.startTimeStep;
	int rate = s.samplingRate;
	int index = 1;

	while (step <= s.stopTimeStep) {
		for (QString prefix : m_wizard->prefixList()) {
			prefix.append(QString("%1").arg(index, len, 10, QChar('0')));
			prefix.append(s.imageExtention);
			QString absolute = QDir(s.exportTargetFolder).absoluteFilePath(prefix);
			m_fileList->addItem(QDir::toNativeSeparators(absolute));
			m_wizard->addFileList(absolute);
		}
		++index;
		step += rate;
	}
	if (s.outputMovie) {
		for (const QString& prefix : m_wizard->prefixList()) {
			auto p2 = prefix;
			p2.replace("_", "");
			QString movieFilename = QString("%1.mp4").arg(p2);
			QString absMovieFilename = QDir(s.exportTargetFolder).absoluteFilePath(movieFilename);
			m_fileList->addItem(QDir::toNativeSeparators(absMovieFilename));
		}
	}
	m_fileList->sortItems();

	auto setting = m_wizard->setting();

	// kml file
	if (setting.outputKml) {
		QString kml = QDir(s.exportTargetFolder).absoluteFilePath(setting.kmlFilename);
		m_fileList->addItem(QDir::toNativeSeparators(kml));
	}
}
