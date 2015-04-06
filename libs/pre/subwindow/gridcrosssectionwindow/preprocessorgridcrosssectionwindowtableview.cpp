#include "preprocessorgridcrosssectionwindowtableview.h"
#include <QResizeEvent>

PreProcessorGridCrosssectionWindowTableView::PreProcessorGridCrosssectionWindowTableView(QWidget *parent) :
	QTableView(parent)
{
	m_editor = 0;
	m_editIndex = QModelIndex();
	setMouseTracking(true);
	setEditTriggers(
		QAbstractItemView::DoubleClicked |
		QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed |
		QAbstractItemView::CurrentChanged);
}

void PreProcessorGridCrosssectionWindowTableView::resizeEvent(QResizeEvent *event)
{
//	int iWidth = indexWidth;
//	int restWidth = (event->size().width() - iWidth);
//	setColumnWidth(0, iWidth);
//	setColumnWidth(1, restWidth);
	setColumnWidth(0, event->size().width());

	QTableView::resizeEvent(event);
}
