#ifndef ABSTRACTCROSSSECTIONWINDOW_SNAPSHOTSAVECONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_SNAPSHOTSAVECONTROLLER_H

#include "../abstractcrosssectionwindow.h"
#include "abstractcrosssectionwindow_exportsettingdialog.h"

#include <QObject>

class AbstractCrosssectionWindow::SnapshotSaveController : public QObject
{
	Q_OBJECT

public:
	SnapshotSaveController(AbstractCrosssectionWindow* window);
	void save();

private slots:
	void cancel();

private:
	QString m_dirName;
	QString m_prefix;

	ExportSettingDialog::CrossSectionMode m_mode;
	int m_min;
	int m_max;
	int m_skipRate;

	bool m_canceled;
	AbstractCrosssectionWindow* m_window;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_SNAPSHOTSAVECONTROLLER_H
