#ifndef ABSTRACTCROSSSECTIONWINDOW_EDITTABLECONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_EDITTABLECONTROLLER_H

#include "../abstractcrosssectionwindow.h"

class QTableView;

class AbstractCrosssectionWindow::EditTableController : public QObject
{
public:
	EditTableController(QTableView* view, Impl* impl);

	const QStandardItemModel& model() const;
	bool saveCsvFile(const QString& fileName);
	void applyToTable();

private:
	void setDataToModel(int col, const GridAttributeDisplaySettingContainer& setting, int valueCount);

	QTableView* m_view;
	Impl* m_impl;

	QStandardItemModel m_model;

	class Delegate;
	Delegate* m_delegate;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_EDITTABLECONTROLLER_H
