#include "geodatariversurveycrosssectionwindowwsetablewidget.h"

#include <misc/mathsupport.h>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>

namespace {

const int TABLE_CHECK_WIDTH = 20;
const int TABLE_NAME_WIDTH = 100;
const int TABLE_REALVAL_WIDTH = 60;
const int TABLE_COLOR_WIDTH = 40;
const int TABLE_ROWHEIGHT = 24;

} // namespace

GeoDataRiverSurveyCrosssectionWindowWseTableWidget::GeoDataRiverSurveyCrosssectionWindowWseTableWidget(QWidget* parent) :
	QTableWidget(parent)
{
	setColumnCount(4);
	QList<QString> labels;
	labels.append("");
	labels.append(tr("WSE Name"));
	labels.append(tr("Elevation"));
	labels.append(tr("Color"));
	setHorizontalHeaderLabels(labels);
	setColumnWidth(0, TABLE_CHECK_WIDTH);
	setColumnWidth(1, TABLE_NAME_WIDTH);
	setColumnWidth(2, TABLE_REALVAL_WIDTH);
	setColumnWidth(3, TABLE_COLOR_WIDTH);
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::SingleSelection);

	setupMenu();
}

void GeoDataRiverSurveyCrosssectionWindowWseTableWidget::mousePressEvent(QMouseEvent* event)
{
	QTableWidget::mousePressEvent(event);
	m_pressPosition = event->pos();
}

void GeoDataRiverSurveyCrosssectionWindowWseTableWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::RightButton) {
		QTableWidget::mouseReleaseEvent(event);
		return;
	}

	if (! iRIC::isNear(m_pressPosition, event->pos())) {return;}

	int selectedRow = selectedIndexes().first().row();

	m_moveUpAction->setEnabled(rowCount() > 1 && selectedRow != 0);
	m_moveDownAction->setEnabled(rowCount() > 1 && selectedRow != rowCount() - 1);
	m_deleteAction->setEnabled(rowCount() > 1);

	m_menu->move(event->globalPos());
	m_menu->show();
}

void GeoDataRiverSurveyCrosssectionWindowWseTableWidget::moveUpSelected()
{
	int selectedRow = selectedIndexes().first().row();

	emit moveUpRequested(selectedRow);
}

void GeoDataRiverSurveyCrosssectionWindowWseTableWidget::moveDownSelected()
{
	int selectedRow = selectedIndexes().first().row();

	emit moveDownRequested(selectedRow);
}

void GeoDataRiverSurveyCrosssectionWindowWseTableWidget::deleteSelected()
{
	int selectedRow = selectedIndexes().first().row();

	emit deleteRequested(selectedRow);
}

void GeoDataRiverSurveyCrosssectionWindowWseTableWidget::setupMenu()
{
	m_moveUpAction = new QAction(QIcon(":/libs/guibase/images/iconItemMoveUp.png"), tr("Move &Up"), this);
	connect(m_moveUpAction, SIGNAL(triggered()), this, SLOT(moveUpSelected()));
	m_moveDownAction = new QAction(QIcon(":/libs/guibase/images/iconItemMoveDown.png"), tr("Move &Down"), this);
	connect(m_moveDownAction, SIGNAL(triggered()), this, SLOT(moveDownSelected()));
	m_deleteAction = new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"), tr("Delete"), this);
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelected()));

	m_menu = new QMenu(this);
	m_menu->addAction(m_moveUpAction);
	m_menu->addAction(m_moveDownAction);
	m_menu->addSeparator();
	m_menu->addAction(m_deleteAction);
}
