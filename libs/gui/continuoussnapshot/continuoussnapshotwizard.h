#ifndef CONTINUOUSSNAPSHOTWIZARD_H
#define CONTINUOUSSNAPSHOTWIZARD_H

#include "continuoussnapshotsetting.h"

#include <QWizard>
#include <QList>

class BackgroundImageInfo;
class iRICMainWindow;
class ContinuousSnapshotWizard;
class CoordinateSystem;

class QMdiSubWindow;

class ContinuousSnapshotWizard : public QWizard
{
	Q_OBJECT

public:
	ContinuousSnapshotWizard(QWidget* parent);
	void accept() override;

	void setSetting(const ContinuousSnapshotSetting& setting);

	ContinuousSnapshotSetting setting() const;

	void setCoordinateSystem(CoordinateSystem* cs) {m_coordinateSystem = cs;}
	void setSnapshotSize(QSize s) {m_snapshotSize = s;}
	void setBeginPosition(QPoint p) {m_beginPosition = p;}
	void setTargetWindow(int i) {m_targetWindow = i;}

	CoordinateSystem* coordinateSystem() const {return m_coordinateSystem;}
	QSize snapshotSize() {return m_snapshotSize;}
	QPoint beginPosition() {return m_beginPosition;}
	int targetWindow() {return m_targetWindow;}

	void clearWindowList() {m_windowList.clear();}
	void addWindowList(QMdiSubWindow* sub) {m_windowList.append(sub);}
	const QList<QMdiSubWindow*>& windowList() {return m_windowList;}
	void clearPrefixList() {m_prefixList.clear();}
	void addPrefixList(const QString& pre) {m_prefixList.append(pre);}
	const QStringList& prefixList() {return m_prefixList;}
	void clearFileList() {m_fileList.clear();}
	void addFileList(const QString& name) {m_fileList.append(name);}
	const QStringList& fileList() {return m_fileList;}

signals:
	void requestChangeCurrentStepIndex(unsigned int);

private:
	iRICMainWindow* m_mainWindow;

	ContinuousSnapshotSetting m_setting;

	// window selection
	QList<QMdiSubWindow*> m_windowList;
	CoordinateSystem* m_coordinateSystem;
	QSize m_snapshotSize;
	QPoint m_beginPosition;     // snapshot top-left position

	// file properties
	QStringList m_prefixList;

	// google earth
	int m_targetWindow;

	// confirm the result
	QStringList m_fileList;
};

#endif // CONTINUOUSSNAPSHOTWIZARD_H
