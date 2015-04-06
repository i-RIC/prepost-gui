#include "rawdatariversurveycrosssectionwindowtableview.h"

#include <QResizeEvent>

RawDataRiverSurveyCrosssectionWindowTableView::RawDataRiverSurveyCrosssectionWindowTableView(QWidget *parent) :
	QTableView(parent)
{
	m_editor = 0;
	m_editIndex = QModelIndex();
	setMouseTracking(true);
	// Set "Active" column width.
	setColumnWidth(0, activeWidth);
	setEditTriggers(
		QAbstractItemView::DoubleClicked |
		QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed |
		QAbstractItemView::CurrentChanged);
}

void RawDataRiverSurveyCrosssectionWindowTableView::resizeEvent(QResizeEvent *event)
{
	int aWidth = activeWidth;
	int restWidth = (event->size().width() - activeWidth - columnMargin) / 3;
	setColumnWidth(0, aWidth);
	setColumnWidth(1, restWidth);
	setColumnWidth(2, restWidth);
	setColumnWidth(3, restWidth);

	QTableView::resizeEvent(event);
}
