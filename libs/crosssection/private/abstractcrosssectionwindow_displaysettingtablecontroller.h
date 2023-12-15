#ifndef ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGTABLECONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGTABLECONTROLLER_H

#include "../abstractcrosssectionwindow.h"

class AbstractCrosssectionWindow::DisplaySettingTableController : public QObject
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

#endif // ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGTABLECONTROLLER_H
