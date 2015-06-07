#include "ui_structured15dgridwithcrosssectioncrosssectionwindow.h"

#include "../../datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include "../../datamodel/preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "../../datamodel/preprocessornormal15dgridwithcrosssectionshapedataitem.h"
#include "structured15dgridwithcrosssectioncrosssectionwindow.h"
#include "structured15dgridwithcrosssectioncrosssectionwindowdelegate.h"
#include "structured15dgridwithcrosssectioncrosssectionwindowprojectdataitem.h"

#include <guicore/pre/grid/structured15dgridwithcrosssection.h>
#include <misc/iricundostack.h>
#include <rawdata/riversurvey/rawdatariverpathpoint.h>

#include <QComboBox>
#include <QLabel>
#include <QStandardItemModel>

Structured15DGridWithCrossSectionCrossSectionWindow::Structured15DGridWithCrossSectionCrossSectionWindow(PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* pdi, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::Structured15DGridWithCrossSectionCrossSectionWindow)
{
	ui->setupUi(this);

	m_shapeItem = item;
	PreProcessorNormal15DGridWithCrossSectionDataItem* di = dynamic_cast<PreProcessorNormal15DGridWithCrossSectionDataItem*>(item->parent());
	m_grid = dynamic_cast<Structured15DGridWithCrossSection*>(di->grid());
	m_settingUp = false;
	m_projectDataItem = pdi;

	m_blackLineCrossSection = 0;

	setupActions();
	setupToolBar();
	setupModel();
	setupView();
	updateComboBoxes();

	connect(m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(handleDataChange()));
}

Structured15DGridWithCrossSectionCrossSectionWindow::~Structured15DGridWithCrossSectionCrossSectionWindow()
{
	delete ui;
	m_projectDataItem->informWindowClose();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::setupActions()
{
	m_deleteAction = new QAction(tr("&Delete"), this);
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedRows()));
}

void Structured15DGridWithCrossSectionCrossSectionWindow::setupToolBar()
{
	ui->toolBar->setFloatable(false);

	QLabel* l;
	l = new QLabel(tr("Black line: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	m_blackLineComboBox = new QComboBox(this);
	m_blackLineComboBox->setMinimumWidth(100);
	ui->toolBar->addWidget(m_blackLineComboBox);

	connect(m_blackLineComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(blackComboBoxChange(int)));
}

void Structured15DGridWithCrossSectionCrossSectionWindow::setupModel()
{
	m_model = new QStandardItemModel(0, 2, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("Position"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Elevation"));

	m_selectionModel = new QItemSelectionModel(m_model, this);
	connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(updateActionStatus()));
}

void Structured15DGridWithCrossSectionCrossSectionWindow::setupView()
{
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);

	Structured15DGridWithCrossSectionCrossSectionWindowDelegate* del = new Structured15DGridWithCrossSectionCrossSectionWindowDelegate();
	ui->tableView->setItemDelegate(del);

	ui->graphicsView->setParentWindow(this);
	ui->graphicsView->setModel(m_model);
	ui->graphicsView->setSelectionModel(m_selectionModel);
}

bool Structured15DGridWithCrossSectionCrossSectionWindow::updateComboBoxes()
{
	m_blackLineComboBox->blockSignals(true);
	m_blackLineComboBox->clear();

	for (auto it = m_grid->crossSections().begin(); it != m_grid->crossSections().end(); ++it) {
		m_blackLineComboBox->addItem((*it)->name());
	}
	if (m_blackLineCrossSection != 0) {
		int index = m_grid->crossSections().indexOf(m_blackLineCrossSection);
		if (index != - 1) {
			m_blackLineComboBox->setCurrentIndex(index);
		} else {
			// maybe this line is deleted.
			m_projectDataItem->requestWindowClose();
			return false;
		}
	}
	m_blackLineComboBox->blockSignals(false);
	return true;
}

void Structured15DGridWithCrossSectionCrossSectionWindow::setTarget(Structured15DGridWithCrossSectionCrossSection* cs)
{
	m_blackLineCrossSection = cs;
	for (auto it = m_grid->crossSections().begin(); it != m_grid->crossSections().end(); ++it) {
		if ((*it) != cs) { continue; }
		break;
	}

	m_blackLineComboBox->blockSignals(true);

	int index = m_grid->crossSections().indexOf(cs);
	if (index != - 1) {
		m_blackLineComboBox->setCurrentIndex(index);
	}

	m_blackLineComboBox->blockSignals(false);

	// set window title.
	PreProcessorGridAndGridCreatingConditionDataItem* di = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*>(m_projectDataItem->parent()->parent()->parent());
	setWindowTitle(QString(tr("%1 : CrossSection %2").arg(di->standardItem()->text()).arg(m_blackLineCrossSection->name())));
	setupData();
}

struct SelectionRange {
	int top;
	int left;
	int bottom;
	int right;
};

void Structured15DGridWithCrossSectionCrossSectionWindow::setupData()
{
	m_settingUp = true;
	// detach table view once.
	ui->tableView->setModel(0);
	// keep selection status.
	QItemSelection selection = m_selectionModel->selection();
	QList<SelectionRange> sel;
	for (auto sit = selection.begin(); sit != selection.end(); ++sit) {
		QItemSelectionRange& tmpr = *sit;
		SelectionRange r;
		r.left = tmpr.left();
		r.top = tmpr.top();
		r.right = tmpr.right();
		r.bottom = tmpr.bottom();
		sel.append(r);
	}

	clear();
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& info = m_blackLineCrossSection->altitudeInfo();
	int row = 0;

	Structured15DGridWithCrossSectionCrossSection::Altitude alt;
	for (auto it = info.begin(); it != info.end(); ++it) {
		alt = *it;
		m_model->insertRow(row);
		m_model->setData(m_model->index(row, 0), QVariant(alt.m_position));
		m_model->setData(m_model->index(row, 1), QVariant(alt.m_height));
		ui->tableView->setRowHeight(row, defaultRowHeight);
		++row;
	}
	// restore selection status.
	// restore selection status.
	selection.clear();
	for (int i = 0; i < sel.count(); ++i) {
		SelectionRange r = sel.at(i);
		selection.append(QItemSelectionRange(m_model->index(r.top, r.left), m_model->index(r.bottom, r.right)));
	}
	m_selectionModel->select(selection, QItemSelectionModel::Select);
	// connect to table view.
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);
	// update tableview row heights.
	for (int i = 0; i < m_model->rowCount(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	m_settingUp = false;
	updateActionStatus();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::clear()
{
	int rows = m_model->rowCount();
	m_model->removeRows(0, rows);
}

void Structured15DGridWithCrossSectionCrossSectionWindow::updateActionStatus()
{
	QModelIndexList rows = m_selectionModel->selectedRows();
	m_deleteAction->setEnabled(rows.count() > 0);
}

void Structured15DGridWithCrossSectionCrossSectionWindow::updateView()
{
	ui->graphicsView->viewport()->update();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraFit()
{
	ui->graphicsView->cameraFit();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraMoveLeft()
{
	ui->graphicsView->cameraMoveLeft();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraMoveRight()
{
	ui->graphicsView->cameraMoveRight();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraMoveUp()
{
	ui->graphicsView->cameraMoveUp();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraMoveDown()
{
	ui->graphicsView->cameraMoveDown();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraZoomIn()
{
	ui->graphicsView->cameraZoomIn();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraZoomOut()
{
	ui->graphicsView->cameraZoomOut();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraZoomInX()
{
	ui->graphicsView->cameraZoomInX();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraZoomOutX()
{
	ui->graphicsView->cameraZoomOutX();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraZoomInY()
{
	ui->graphicsView->cameraZoomInY();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::cameraZoomOutY()
{
	ui->graphicsView->cameraZoomOutY();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::handleDataChange()
{
	if (m_settingUp) { return; }
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude> before, after;
	before = m_blackLineCrossSection->altitudeInfo();
	syncData();
	after = m_blackLineCrossSection->altitudeInfo();
	iRICUndoStack::instance().push(new Structured15DGridCrossSectionEditCommand(false, tr("Edit Elevation Point"), after, before, this, m_shapeItem, true));
}

void Structured15DGridWithCrossSectionCrossSectionWindow::syncData()
{
	// update the crosssection.
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& info = m_blackLineCrossSection->altitudeInfo();
	info.clear();
	for (int i = 0; i < m_model->rowCount(); ++i) {
		Structured15DGridWithCrossSectionCrossSection::Altitude alt;
		alt.m_position = m_model->data(m_model->index(i, 0)).toDouble();
		alt.m_height = m_model->data(m_model->index(i, 1)).toDouble();
		info.push_back(alt);
	}
	updateView();
}

void Structured15DGridWithCrossSectionCrossSectionWindow::blackComboBoxChange(int newindex)
{
	auto it = m_grid->crossSections().begin();
	setTarget(*(it + newindex));
}

void Structured15DGridWithCrossSectionCrossSectionWindow::deleteSelectedRows()
{
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude> before, after;
	before = m_blackLineCrossSection->altitudeInfo();
	QModelIndexList rows = m_selectionModel->selectedRows();
	int removedRows = 0;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		int row = index.row() - removedRows;
		m_model->removeRow(row);
		++removedRows;
	}
	syncData();
	after = m_blackLineCrossSection->altitudeInfo();
	iRICUndoStack::instance().push(new Structured15DGridCrossSectionEditCommand(false, tr("Edit Elevation Point"), after, before, this, m_shapeItem, true));
	m_selectionModel->clear();
}

Structured15DGridCrossSectionEditCommand::Structured15DGridCrossSectionEditCommand(bool apply, const QString& title, const QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& after, const QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& before, Structured15DGridWithCrossSectionCrossSectionWindow* w, PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, bool tableaction)
	: QUndoCommand(title)
{
	m_apply = apply;
	m_before = before;
	m_after = after;
	m_window = w;
	m_item = item;
	m_tableaction = tableaction;
	m_first = true;
}

void Structured15DGridCrossSectionEditCommand::redo()
{
	m_window->m_blackLineCrossSection->setAltitudeInfo(m_after);
//	m_point->updateXSecInterpolators();
	if (m_apply || (m_tableaction && m_first)) {
		m_window->updateView();
	} else {
		m_item->updateCrossSectionWindows();
	}
	m_first = false;
}

void Structured15DGridCrossSectionEditCommand::undo()
{
	m_window->m_blackLineCrossSection->setAltitudeInfo(m_before);
//	m_point->updateXSecInterpolators();
	if (! m_apply) {
		m_item->updateCrossSectionWindows();
	}
}
