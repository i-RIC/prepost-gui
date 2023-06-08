#include "../main/iricmainwindow.h"
#include "continuoussnapshotwizard.h"
#include "private/continuoussnapshotwizard_confirmpage.h"
#include "private/continuoussnapshotwizard_filepropertypage.h"
#include "private/continuoussnapshotwizard_googleearthsettingpage.h"
#include "private/continuoussnapshotwizard_introductionpage.h"
#include "private/continuoussnapshotwizard_moviepropertypage.h"
#include "private/continuoussnapshotwizard_timestepsettingpage.h"
#include "private/continuoussnapshotwizard_windowselectionpage.h"

#include <guicore/base/snapshotenabledwindowinterface.h>

#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QVBoxLayout>

ContinuousSnapshotWizard::ContinuousSnapshotWizard(QWidget* parent) :
	QWizard(parent),
	m_projectMainFile {nullptr}
{
	addPage(new IntroductionPage(this));
	addPage(new WindowSelectionPage(this));
	addPage(new FilePropertyPage(this));
	addPage(new MoviePropertyPage(this));
	addPage(new TimestepSettingPage(this));
	addPage(new GoogleEarthSettingPage(this));
	addPage(new ConfirmPage(this));
	setWindowTitle(tr("Continuous Snapshot / Movie / Google Earth Export Wizard"));
	resize(600, 500);

	m_mainWindow = dynamic_cast<iRICMainWindow*>(parent);
}

void ContinuousSnapshotWizard::accept()
{
	QWizard::accept();
}

void ContinuousSnapshotWizard::setSetting(const ContinuousSnapshotSetting& setting)
{
	m_setting = setting;
}

ContinuousSnapshotSetting ContinuousSnapshotWizard::setting() const
{
	return m_setting;
}

ProjectMainFile* ContinuousSnapshotWizard::projectMainFile() const
{
	return m_projectMainFile;
}

void ContinuousSnapshotWizard::setProjectMainFile(ProjectMainFile* mainFile)
{
	m_projectMainFile = mainFile;
}

void ContinuousSnapshotWizard::setSnapshotSize(QSize s)
{
	m_snapshotSize = s;
}

void ContinuousSnapshotWizard::setBeginPosition(const QPoint& p)
{
	m_beginPosition = p;
}

void ContinuousSnapshotWizard::setTargetWindow(int i)
{
	m_targetWindow = i;
}

QSize ContinuousSnapshotWizard::snapshotSize() const
{
	return m_snapshotSize;
}

QPoint ContinuousSnapshotWizard::beginPosition() const
{
	return m_beginPosition;
}

int ContinuousSnapshotWizard::targetWindow() const
{
	return m_targetWindow;
}

void ContinuousSnapshotWizard::addWindowList(QMdiSubWindow* sub)
{
	m_windowList.append(sub);
}

void ContinuousSnapshotWizard::clearWindowList()
{
	m_windowList.clear();
}

const QList<QMdiSubWindow*>& ContinuousSnapshotWizard::windowList() const
{
	return m_windowList;
}

void ContinuousSnapshotWizard::addPrefixList(const QString& pre)
{
	m_prefixList.append(pre);
}

void ContinuousSnapshotWizard::clearPrefixList()
{
	m_prefixList.clear();
}

const QStringList& ContinuousSnapshotWizard::prefixList() const
{
	return m_prefixList;
}

void ContinuousSnapshotWizard::clearFileList()
{
	m_fileList.clear();
}

void ContinuousSnapshotWizard::addFileList(const QString& name)
{
	m_fileList.append(name);
}

const QStringList& ContinuousSnapshotWizard::fileList() const
{
	return m_fileList;
}

QStringList ContinuousSnapshotWizard::getMovieProfile(ContinuousSnapshotSetting::MovieProfile profile)
{
	return MoviePropertyPage::getProfile(profile);
}
