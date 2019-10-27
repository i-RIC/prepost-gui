#include "ui_preprocessorgridcrosssectionwindow.h"

#include "preprocessorgridcrosssectionwindow.h"
#include "preprocessorgridcrosssectionwindowdelegate.h"
#include "preprocessorgridcrosssectionwindowprojectdataitem.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QCloseEvent>
#include <QComboBox>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMessageBox>
#include <QModelIndexList>
#include <QRadioButton>
#include <QStandardItemModel>

const int PreProcessorGridCrosssectionWindow::DEFAULT_TABLE_WIDTH = 200;

PreProcessorGridCrosssectionWindow::PreProcessorGridCrosssectionWindow(Structured2DGrid* grid, const std::string& condName, PreProcessorGridCrosssectionWindowProjectDataItem* pdi, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::PreProcessorGridCrosssectionWindow),
	m_condition (condName)
{
	m_grid = grid;
	m_settingUp = false;
	m_projectDataItem = pdi;

	m_direction = dirI;

	m_blackLineIndex = 0;
	m_redLineIndex = 0;
	m_blueLineIndex = 0;

	ui->setupUi(this);

	m_icon = QIcon(":/libs/geodata/riversurvey/images/iconRiverCrosssection.png");

	QList<int> sizes;
	int width = size().width();
	sizes.append(width - DEFAULT_TABLE_WIDTH);
	sizes.append(DEFAULT_TABLE_WIDTH);

	ui->splitter->setSizes(sizes);

	setupActions();
	setupToolBar();
	setupModel();
	setupView();
	updateComboBoxes();

	connect(m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(handleDataChange()));
}

void PreProcessorGridCrosssectionWindow::setupActions()
{

}

PreProcessorGridCrosssectionWindow::~PreProcessorGridCrosssectionWindow()
{
	delete ui;
	m_projectDataItem->informWindowClose();
}

void PreProcessorGridCrosssectionWindow::setupToolBar()
{
	ui->toolBar->setFloatable(false);

	QLabel* l;
	QWidget* spacer;
	l = new QLabel(tr("Direction: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	m_iRadioButton = new QRadioButton("I", this);
	ui->toolBar->addWidget(m_iRadioButton);
	m_jRadioButton = new QRadioButton("J", this);
	ui->toolBar->addWidget(m_jRadioButton);

	spacer = new QWidget(ui->toolBar);
	spacer->setFixedWidth(10);
	ui->toolBar->addWidget(spacer);

	l = new QLabel(tr("Black line: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	m_blackLineComboBox = new QComboBox(this);
	m_blackLineComboBox->setMinimumWidth(100);
	ui->toolBar->addWidget(m_blackLineComboBox);

	spacer = new QWidget(ui->toolBar);
	spacer->setFixedWidth(10);
	ui->toolBar->addWidget(spacer);

	l = new QLabel(tr("Red line: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	m_redLineComboBox = new QComboBox(this);
	m_redLineComboBox->setMinimumWidth(100);
	ui->toolBar->addWidget(m_redLineComboBox);

	spacer = new QWidget(ui->toolBar);
	spacer->setFixedWidth(10);
	ui->toolBar->addWidget(spacer);

	l = new QLabel(tr("Blue line: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	m_blueLineComboBox = new QComboBox(this);
	m_blueLineComboBox->setMinimumWidth(100);
	ui->toolBar->addWidget(m_blueLineComboBox);

	connect(m_iRadioButton, SIGNAL(toggled(bool)), this, SLOT(toggleI(bool)));
	connect(m_jRadioButton, SIGNAL(toggled(bool)), this, SLOT(toggleJ(bool)));
	connect(m_blackLineComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(blackComboBoxChange(int)));
	connect(m_redLineComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(redComboBoxChange(int)));
	connect(m_blueLineComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(blueComboBoxChange(int)));
}

void PreProcessorGridCrosssectionWindow::setupModel()
{
	GridAttributeContainer* cont = m_grid->gridAttribute(m_condition);
	m_model = new QStandardItemModel(0, 1, this);
	m_model->setHeaderData(0, Qt::Horizontal, cont->gridAttribute()->caption());

	m_selectionModel = new QItemSelectionModel(m_model, this);
	connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(updateActionStatus()));
}

bool PreProcessorGridCrosssectionWindow::updateComboBoxes()
{
	m_blackLineComboBox->blockSignals(true);
	m_redLineComboBox->blockSignals(true);
	m_blueLineComboBox->blockSignals(true);

	m_blackLineComboBox->clear();
	m_redLineComboBox->clear();
	m_blueLineComboBox->clear();
	// add "no data" to previous, next.
	m_redLineComboBox->addItem(tr("(No data)"));
	m_blueLineComboBox->addItem(tr("(No data)"));

	int maxIndex;
	QString label;
	if (m_direction == dirI) {
		maxIndex = m_grid->dimensionJ();
		label = "J = %1";
	} else if (m_direction == dirJ) {
		maxIndex = m_grid->dimensionI();
		label = "I = %1";
	}
	for (int index = 1; index <= maxIndex; ++index) {
		QString l = label.arg(index);
		m_blackLineComboBox->addItem(l);
		m_redLineComboBox->addItem(l);
		m_blueLineComboBox->addItem(l);
	}

	m_blackLineComboBox->blockSignals(false);
	m_redLineComboBox->blockSignals(false);
	m_blueLineComboBox->blockSignals(false);
	return true;
}

void PreProcessorGridCrosssectionWindow::setTarget(PreProcessorGridCrosssectionWindow::Direction dir, int index)
{
	m_direction = dir;
	if (m_direction == dirI) {
		m_iRadioButton->blockSignals(true);
		m_iRadioButton->setChecked(true);
		m_iRadioButton->blockSignals(false);
	} else {
		m_jRadioButton->blockSignals(true);
		m_jRadioButton->setChecked(true);
		m_jRadioButton->blockSignals(false);
	}
	updateComboBoxes();

	m_blackLineIndex = index;

	if (m_blackLineIndex > 0) {
		m_redLineIndex = m_blackLineIndex - 1;
	} else {
		m_redLineIndex = - 1;
	}
	int dim;
	QString title;
	if (m_direction == PreProcessorGridCrosssectionWindow::dirI) {
		dim = m_grid->dimensionJ();
		title = tr("Grid Crosssection (%1): J = %2");
	} else {
		dim = m_grid->dimensionI();
		title = tr("Grid Crosssection (%1): I = %2");
	}
	if (m_blackLineIndex < dim - 1) {
		m_blueLineIndex = m_blackLineIndex + 1;
	} else {
		m_blueLineIndex = - 1;
	}
	m_blackLineComboBox->blockSignals(true);
	m_redLineComboBox->blockSignals(true);
	m_blueLineComboBox->blockSignals(true);

	m_blackLineComboBox->setCurrentIndex(m_blackLineIndex);
	m_redLineComboBox->setCurrentIndex(m_redLineIndex + 1);
	m_blueLineComboBox->setCurrentIndex(m_blueLineIndex + 1);

	m_blackLineComboBox->blockSignals(false);
	m_redLineComboBox->blockSignals(false);
	m_blueLineComboBox->blockSignals(false);

	// set window title.
	QString caption = m_grid->gridAttribute(m_condition)->gridAttribute()->caption();
	setWindowTitle(title.arg(caption).arg(m_blackLineIndex + 1));
	setupData();
}

const std::string& PreProcessorGridCrosssectionWindow::condition() const
{
	return m_condition;
}

PreProcessorGridCrosssectionWindow::Direction PreProcessorGridCrosssectionWindow::targetDirection() const
{
	return m_direction;
}

int PreProcessorGridCrosssectionWindow::targetIndex() const
{
	return m_blackLineIndex;
}

struct SelectionRange {
	int top;
	int left;
	int bottom;
	int right;
};

void PreProcessorGridCrosssectionWindow::setupData()
{
	m_settingUp = true;
	// detach table view once.
	ui->tableView->setModel(0);
	// keep selection status
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
	GridAttributeRealNodeContainer* cont =
		dynamic_cast<GridAttributeRealNodeContainer*>(m_grid->gridAttribute(m_condition));
	int row = 0;
	if (m_direction == dirI) {
		for (unsigned int i = 0; i < m_grid->dimensionI(); ++i) {
			double val = cont->value(m_grid->vertexIndex(i, targetIndex()));
			m_model->insertRow(row);
			m_model->setData(m_model->index(row, 0), val);
			m_model->setHeaderData(row, Qt::Vertical, QString("(%1, %2)").arg(i + 1).arg(targetIndex() + 1));

			ui->tableView->setRowHeight(row, defaultRowHeight);
			++row;
		}
	} else {
		for (unsigned int j = 0; j < m_grid->dimensionJ(); ++j) {
			double val = cont->value(m_grid->vertexIndex(targetIndex(), j));
			m_model->insertRow(row);
			m_model->setData(m_model->index(row, 0), val);
			m_model->setHeaderData(row, Qt::Vertical, QString("(%1, %2)").arg(targetIndex() + 1).arg(j + 1));

			ui->tableView->setRowHeight(row, defaultRowHeight);
			++row;
		}
	}

	// connect to table view.
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);
	// restore selection status.
	selection.clear();
	for (int i = 0; i < sel.count(); ++i) {
		SelectionRange r = sel.at(i);
		selection.append(QItemSelectionRange(m_model->index(r.top, r.left), m_model->index(r.bottom, r.right)));
	}
	m_selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Clear);
	// update tableview row heights.
	for (int i = 0; i < m_model->rowCount(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	m_settingUp = false;
	updateActionStatus();
}

void PreProcessorGridCrosssectionWindow::setupView()
{
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);

	PreProcessorGridCrosssectionWindowDelegate* del = new PreProcessorGridCrosssectionWindowDelegate();
//	del->setCrosssection(&(m_blackLinePoint->crosssection()));
	ui->tableView->setItemDelegate(del);

	ui->graphicsView->setParentWindow(this);
	ui->graphicsView->setModel(m_model);
	ui->graphicsView->setSelectionModel(m_selectionModel);
}

void PreProcessorGridCrosssectionWindow::clear()
{
	int rows = m_model->rowCount();
	m_model->removeRows(0, rows);
}

void PreProcessorGridCrosssectionWindow::updateView()
{
	ui->graphicsView->viewport()->update();
}

void PreProcessorGridCrosssectionWindow::cameraFit()
{
	ui->graphicsView->cameraFit();
}

void PreProcessorGridCrosssectionWindow::cameraMoveLeft()
{
	ui->graphicsView->cameraMoveLeft();
}

void PreProcessorGridCrosssectionWindow::cameraMoveRight()
{
	ui->graphicsView->cameraMoveRight();
}

void PreProcessorGridCrosssectionWindow::cameraMoveUp()
{
	ui->graphicsView->cameraMoveUp();
}

void PreProcessorGridCrosssectionWindow::cameraMoveDown()
{
	ui->graphicsView->cameraMoveDown();
}

void PreProcessorGridCrosssectionWindow::cameraZoomIn()
{
	ui->graphicsView->cameraZoomIn();
}

void PreProcessorGridCrosssectionWindow::cameraZoomOut()
{
	ui->graphicsView->cameraZoomOut();
}

void PreProcessorGridCrosssectionWindow::cameraZoomInX()
{
	ui->graphicsView->cameraZoomInX();
}

void PreProcessorGridCrosssectionWindow::cameraZoomOutX()
{
	ui->graphicsView->cameraZoomOutX();
}

void PreProcessorGridCrosssectionWindow::cameraZoomInY()
{
	ui->graphicsView->cameraZoomInY();
}

void PreProcessorGridCrosssectionWindow::cameraZoomOutY()
{
	ui->graphicsView->cameraZoomOutY();
}

void PreProcessorGridCrosssectionWindow::handleDataChange()
{
	if (m_settingUp) {return;}
	syncData();
}

bool PreProcessorGridCrosssectionWindow::syncData()
{
	GridAttributeRealNodeContainer* cont =
		dynamic_cast<GridAttributeRealNodeContainer*>(m_grid->gridAttribute(m_condition));
	if (m_direction == dirI) {
		for (int i = 0; i < m_model->rowCount(); ++i) {
			double data = m_model->data(m_model->index(i, 0)).toDouble();
			cont->setValue(m_grid->vertexIndex(i, targetIndex()), data);
		}
	} else {
		for (int j = 0; j < m_model->rowCount(); ++j) {
			double data = m_model->data(m_model->index(j, 0)).toDouble();
			cont->setValue(m_grid->vertexIndex(targetIndex(), j), data);
		}
	}
	cont->dataArray()->Modified();
	m_grid->setModified();
	cont->setCustomModified(true);
	updateView();
	m_settingUp = false;
	return true;
}

void PreProcessorGridCrosssectionWindow::blackComboBoxChange(int newindex)
{
	setTarget(m_direction, newindex);
	informFocusIn();
}

void PreProcessorGridCrosssectionWindow::redComboBoxChange(int newindex)
{
	m_redLineIndex = newindex - 1;
	/*
		if (newindex == 0){
			m_redLinePoint = nullptr;
		}else{
			m_redLinePoint = m_riverSurvey->headPoint();
			for (int i = 0; i < newindex; ++i){
				m_redLinePoint = m_redLinePoint->nextPoint();
			}
		}
	*/
	ui->graphicsView->viewport()->update();
	informFocusIn();
}

void PreProcessorGridCrosssectionWindow::blueComboBoxChange(int newindex)
{
	m_blueLineIndex = newindex - 1;
	/*
		if (newindex == 0){
			m_blueLinePoint = nullptr;
		}else{
			m_blueLinePoint = m_riverSurvey->headPoint();
			for (int i = 0; i < newindex; ++i){
				m_blueLinePoint = m_blueLinePoint->nextPoint();
			}
		}
	*/
	ui->graphicsView->viewport()->update();
	informFocusIn();
}

void PreProcessorGridCrosssectionWindow::toggleI(bool toggled)
{
	if (! toggled) {return;}
	setTarget(dirI, 0);
	cameraFit();
}

void PreProcessorGridCrosssectionWindow::toggleJ(bool toggled)
{
	if (! toggled) {return;}
	setTarget(dirJ, 0);
	cameraFit();
}

void PreProcessorGridCrosssectionWindow::updateActionStatus()
{
//	QModelIndexList rows = m_selectionModel->selectedRows();
//	m_deleteAction->setEnabled(rows.count() > 0);
}


/*
GeoDataRiverSurveyCrosssectionEditCommand::GeoDataRiverSurveyCrosssectionEditCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, PreProcessorGridCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction)
	: QUndoCommand(title)
{
	m_apply = apply;
	m_point = p;
	m_before = before;
	m_after = after;
	m_window = w;
	m_rs = rs;
	m_tableaction = tableaction;
	m_first = true;
}

void GeoDataRiverSurveyCrosssectionEditCommand::redo()
{
	m_point->crosssection().AltitudeInfo() = m_after;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (m_apply || (m_tableaction && m_first)){
		m_window->updateView();
	}else{
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	m_first = false;
}

void GeoDataRiverSurveyCrosssectionEditCommand::undo()
{
	m_point->crosssection().AltitudeInfo() = m_before;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (! m_apply){
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
}
*/

QTableView* PreProcessorGridCrosssectionWindow::tableView() const
{
	return ui->tableView;
}

void PreProcessorGridCrosssectionWindow::informFocusIn()
{
}

Structured2DGrid* PreProcessorGridCrosssectionWindow::grid() const
{
	return m_grid;
}

void PreProcessorGridCrosssectionWindow::update()
{
	ui->graphicsView->viewport()->update();
	QWidget::update();
}

PreProcessorGridAttributeNodeDataItem* PreProcessorGridCrosssectionWindow::conditionNodeDataItem()
{
	return m_projectDataItem->conditionNodeDataItem();
}


void PreProcessorGridCrosssectionWindow::informSelectedVerticesChanged(const QVector<vtkIdType>& vertices)
{
	ui->graphicsView->informSelectedVerticesChanged(vertices);
}
