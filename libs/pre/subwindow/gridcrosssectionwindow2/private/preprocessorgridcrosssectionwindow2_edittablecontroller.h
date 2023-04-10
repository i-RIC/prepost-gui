#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_EDITTABLECONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_EDITTABLECONTROLLER_H

#include "../preprocessorgridcrosssectionwindow2.h"

class PreProcessorGridCrosssectionWindow2::EditTableController : public QObject
{
public:
	EditTableController(QTableView* view, Impl* impl);

	const QStandardItemModel& model() const;
	bool saveCsvFile(const QString& fileName);
	void applyToTable();

private:
	QTableView* m_view;
	Impl* m_impl;

	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_EDITTABLECONTROLLER_H
