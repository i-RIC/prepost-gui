#ifndef ABSTRACTCROSSSECTIONWINDOW_CSVEXPORTCONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_CSVEXPORTCONTROLLER_H

#include "../abstractcrosssectionwindow.h"
#include "abstractcrosssectionwindow_exportsettingdialog.h"

#include <QObject>

class AbstractCrosssectionWindow::CsvExportController : public QObject
{
	Q_OBJECT

public:
	CsvExportController(AbstractCrosssectionWindow* window);
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


#endif // ABSTRACTCROSSSECTIONWINDOW_CSVEXPORTCONTROLLER_H
