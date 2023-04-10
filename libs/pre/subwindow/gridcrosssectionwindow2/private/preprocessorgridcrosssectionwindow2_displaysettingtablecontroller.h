#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGTABLECONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGTABLECONTROLLER_H

#include "../preprocessorgridcrosssectionwindow2.h"

class PreProcessorGridCrosssectionWindow2::DisplaySettingTableController : public QObject
{
	Q_OBJECT

public:
	DisplaySettingTableController(QTableView* view, Impl* impl);

	void applyToTable();
	void updateVisible();

private slots:
	void handleItemClick(const QModelIndex& index);

private:
	QTableView* m_view;
	Impl* m_impl;
	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGTABLECONTROLLER_H
