#ifndef CONTINUOUSSNAPSHOTWIZARD_H
#define CONTINUOUSSNAPSHOTWIZARD_H

#include "continuoussnapshotsetting.h"

#include <QWizard>
#include <QList>

class iRICMainWindow;
class ProjectMainFile;

class QMdiSubWindow;

class ContinuousSnapshotWizard : public QWizard
{
	Q_OBJECT

public:
	ContinuousSnapshotWizard(QWidget* parent);
	void accept() override;

	void setSetting(const ContinuousSnapshotSetting& setting);

	ContinuousSnapshotSetting setting() const;

	ProjectMainFile* projectMainFile() const;
	void setProjectMainFile(ProjectMainFile* mainFile);

	void setSnapshotSize(QSize s);
	void setBeginPosition(const QPoint& p);
	void setTargetWindow(int i);

	QSize snapshotSize() const;
	QPoint beginPosition() const;

	int targetWindow() const;
	void addWindowList(QMdiSubWindow* sub);
	void clearWindowList();
	const QList<QMdiSubWindow*>& windowList() const;

	void addPrefixList(const QString& pre);
	void clearPrefixList();
	const QStringList& prefixList() const;

	void clearFileList();
	void addFileList(const QString& name);
	const QStringList& fileList() const;

	static QStringList getMovieProfile(ContinuousSnapshotSetting::MovieProfile profile);

signals:
	void requestChangeCurrentStepIndex(unsigned int);

private:
	iRICMainWindow* m_mainWindow;

	ContinuousSnapshotSetting m_setting;

	// window selection
	QList<QMdiSubWindow*> m_windowList;
	ProjectMainFile* m_projectMainFile;
	QSize m_snapshotSize;
	QPoint m_beginPosition;     // snapshot top-left position

	// file properties
	QStringList m_prefixList;

	// google earth
	int m_targetWindow;

	// confirm the result
	QStringList m_fileList;

	class ConfirmPage;
	class FilePropertyPage;
	class GoogleEarthSettingPage;
	class IntroductionPage;
	class MoviePropertyPage;
	class TimestepSettingPage;
	class WindowSelectionPage;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_H
