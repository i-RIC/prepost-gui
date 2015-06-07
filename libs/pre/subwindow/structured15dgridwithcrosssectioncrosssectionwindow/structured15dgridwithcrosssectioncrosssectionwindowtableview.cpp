#include "structured15dgridwithcrosssectioncrosssectionwindowtableview.h"

#include <QResizeEvent>

Structured15DGridWithCrossSectionCrossSectionWindowTableView::Structured15DGridWithCrossSectionCrossSectionWindowTableView(QWidget* parent)
	: QTableView(parent)
{}

void Structured15DGridWithCrossSectionCrossSectionWindowTableView::resizeEvent(QResizeEvent* event)
{
	int restWidth = (event->size().width() - columnMargin) / 2;
	setColumnWidth(0, restWidth);
	setColumnWidth(1, restWidth);
}
