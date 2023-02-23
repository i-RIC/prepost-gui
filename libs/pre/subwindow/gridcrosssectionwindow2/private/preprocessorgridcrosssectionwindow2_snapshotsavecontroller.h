#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_SNAPSHOTSAVECONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_SNAPSHOTSAVECONTROLLER_H

#include "../preprocessorgridcrosssectionwindow2.h"
#include "preprocessorgridcrosssectionwindow2_exportsettingdialog.h"

#include <QObject>

class PreProcessorGridCrosssectionWindow2::SnapshotSaveController : public QObject
{
	Q_OBJECT

public:
	SnapshotSaveController(PreProcessorGridCrosssectionWindow2* window);
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
	PreProcessorGridCrosssectionWindow2* m_window;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_SNAPSHOTSAVECONTROLLER_H
