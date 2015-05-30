#include "ui_rawdatariversurveycrosssectionwindow.h"

#include "rawdatarivercrosssection.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveycrosssectionwindow.h"
#include "rawdatariversurveycrosssectionwindowdelegate.h"
#include "rawdatariversurveycrosssectionwindowprojectdataitem.h"

#include <guibase/centeredcheckbox.h>
#include <guibase/coloreditwidget.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QCloseEvent>
#include <QColorDialog>
#include <QComboBox>
#include <QItemDelegate>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndexList>
#include <QPainter>
#include <QStandardItemModel>
#include <QUndoGroup>

class RawDataRiverSurveyCrosssectionWindowTableDelegate : public QItemDelegate
{
public:
	RawDataRiverSurveyCrosssectionWindowTableDelegate(QObject* parent = nullptr): QItemDelegate(parent) {}
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
		if (index.column() == 0) {
			QVariant dat = index.model()->data(index, Qt::DisplayRole);
			CenteredCheckBox checkbox(nullptr);
			checkbox.setChecked(dat.toBool());
			checkbox.resize(option.rect.size());
			QPixmap pixmap(option.rect.size());
			checkbox.render(&pixmap);
			painter->drawPixmap(option.rect, pixmap);
		} else if (index.column() == 2) {
			QVariant dat = index.model()->data(index, Qt::DisplayRole);
			QColor col = dat.value<QColor>();
			QBrush brush(col);
			painter->fillRect(option.rect, brush);
		} else {
			QItemDelegate::paint(painter, option, index);
		}
	}
};


RawDataRiverSurveyCrosssectionWindow::RawDataRiverSurveyCrosssectionWindow(PreProcessorRawDataGroupDataItemInterface* gitem, RawDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::RawDataRiverSurveyCrosssectionWindow)
{
	m_groupDataItem = gitem;
	m_settingUp = false;
	m_projectDataItem = pdi;

	m_targetRiverSurvey = nullptr;
	m_editTargetPoint = nullptr;
	m_gridCreatingConditionRiverSurvey = nullptr;
	m_gridCreatingConditionPoint = nullptr;

	m_icon = QIcon(":/libs/rawdata/riversurvey/images/iconRiverCrosssection.png");
	m_colorSource = new ColorSource(nullptr);

	ui->setupUi(this);
	QList<int> hSizes;
	hSizes.append(600);
	hSizes.append(200);
	ui->horizontalSplitter->setSizes(hSizes);
	QList<int> vSizes;
	vSizes.append(200);
	vSizes.append(600);
	ui->verticalSplitter->setSizes(vSizes);

	setupActions();
	setupToolBar();
	setupModel();
	setupView();
	setupMenu();
	setupSurveyTable();
	updateRiverSurveys();

	connect(m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(handleDataChange()));
	connect(ui->surveysTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleSurveyTableItemClick(QTableWidgetItem*)));
	connect(ui->surveysTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleSurveyTableItemEdit(QTableWidgetItem*)));
	connect(ui->surveysTableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(handleSurveyTablecurrentCellChange(int,int,int,int)));
}


RawDataRiverSurveyCrosssectionWindow::~RawDataRiverSurveyCrosssectionWindow()
{
	delete ui;
	delete m_colorSource;
	m_projectDataItem->informWindowClose();
}

void RawDataRiverSurveyCrosssectionWindow::setupActions()
{
	m_inactivateByWEOnlyThisAction = new QAction(tr("&This cross-section only"), this);
	m_inactivateByWEAllAction = new QAction(tr("All cross-sections"), this);
	m_deleteAction = new QAction(tr("&Delete"), this);

	connect(m_inactivateByWEOnlyThisAction, SIGNAL(triggered()), this, SLOT(inactivateByWEOnlyThis()));
	connect(m_inactivateByWEAllAction, SIGNAL(triggered()), this, SLOT(inactivateByWEAll()));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedRows()));
}

void RawDataRiverSurveyCrosssectionWindow::setupMenu()
{
	m_elevationPointMenu = new QMenu(tr("&Elevation Point"));
	RawDataRiverSurveyCrosssectionWindowGraphicsView* gview = ui->graphicsView;
	m_elevationPointMenu->addAction(gview->activateAction());
	m_elevationPointMenu->addAction(gview->inactivateAction());

	m_elevationPointMenu->addSeparator();
	QMenu* submenu = m_elevationPointMenu->addMenu(tr("Inactivate using &water elevation"));
	submenu->addAction(m_inactivateByWEOnlyThisAction);
	submenu->addAction(m_inactivateByWEAllAction);

	m_elevationPointMenu->addSeparator();
	m_elevationPointMenu->addAction(gview->moveAction());
	m_elevationPointMenu->addAction(m_deleteAction);
}

void RawDataRiverSurveyCrosssectionWindow::setupToolBar()
{
	ui->toolBar->setFloatable(false);

	QLabel* l;
	QWidget* spacer;
	l = new QLabel(tr("Crosssection: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	m_crosssectionComboBox = new QComboBox(this);
	m_crosssectionComboBox->setMinimumWidth(100);
	ui->toolBar->addWidget(m_crosssectionComboBox);

	spacer = new QWidget(ui->toolBar);
	spacer->setFixedWidth(10);
	ui->toolBar->addWidget(spacer);

	ui->weSpinBox->setEnabled(false);

	connect(m_crosssectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(crosssectionComboBoxChange(int)));
	connect(ui->weCheckBox, SIGNAL(toggled(bool)), this, SLOT(weCheckboxChange(bool)));
	connect(ui->weSpinBox, SIGNAL(valueChanged(double)), this, SLOT(weValueChange(double)));
}

void RawDataRiverSurveyCrosssectionWindow::setupModel()
{
	m_model = new QStandardItemModel(0, 4, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("Active"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Distance from left bank"));
	m_model->setHeaderData(2, Qt::Horizontal, tr("Position"));
	m_model->setHeaderData(3, Qt::Horizontal, tr("Elevation"));

	m_selectionModel = new QItemSelectionModel(m_model, this);
	connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(updateActionStatus()));
}

void RawDataRiverSurveyCrosssectionWindow::updateComboBoxes()
{
	// update crosssectionNames.
	QMap<double, bool> tmpNames;
	for (int i = 0; i < m_riverSurveys.count(); ++i) {
		RawDataRiverSurvey* rs = m_riverSurveys.at(i);
		RawDataRiverPathPoint* p = rs->headPoint();
		p = p->nextPoint();
		while (p != nullptr) {
			tmpNames.insert(p->name().toDouble(), false);
			p = p->nextPoint();
		}
	}
	m_crosssectionNames = tmpNames.keys();

	m_crosssectionComboBox->blockSignals(true);
	m_crosssectionComboBox->clear();
	for (int i = 0; i < m_crosssectionNames.count(); ++i) {
		QString name = QString("%1").arg(m_crosssectionNames.at(i));
		m_crosssectionComboBox->addItem(name);
	}
	m_crosssectionComboBox->blockSignals(false);

	int index = m_crosssectionNames.indexOf(m_crosssectionName);
	if (index != - 1) {
		m_crosssectionComboBox->blockSignals(true);
		m_crosssectionComboBox->setCurrentIndex(index);
		m_crosssectionComboBox->blockSignals(false);
		updateEditTargetPoint();
	} else {
		if (m_crosssectionNames.count() != 0) {
			setCrosssection(m_crosssectionNames[0]);
		} else {
			setCrosssection(0);
		}
	}
}

void RawDataRiverSurveyCrosssectionWindow::setRiverSurvey(RawDataRiverSurvey* rs)
{
	m_targetRiverSurvey = rs;
}

void RawDataRiverSurveyCrosssectionWindow::setCrosssection(double crosssection)
{
	m_crosssectionName = crosssection;
	updateRiverPathPoints();

	m_crosssectionComboBox->blockSignals(true);
	int index = m_crosssectionNames.indexOf(crosssection);
	if (index != - 1) {
		m_crosssectionComboBox->setCurrentIndex(index);
	}
	m_crosssectionComboBox->blockSignals(false);

	updateEditTargetPoint();
	// set window title.
	setWindowTitle(QString(tr("Crosssection Window : %2").arg(crosssection)));
	// fit the camera.
	cameraFit();
}

struct SelectionRange {
	int top;
	int left;
	int bottom;
	int right;
};

void RawDataRiverSurveyCrosssectionWindow::setupData()
{
	m_settingUp = true;
	// detach table view once.
	ui->tableView->setModel(nullptr);
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

	if (m_editTargetPoint == nullptr) {return;}

	RawDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	RawDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	int row = 0;

	RawDataRiverCrosssection::Altitude alt;
	for (auto it = alist.begin(); it != alist.end(); ++it) {
		alt = *it;
		m_model->insertRow(row);
		m_model->setData(m_model->index(row, 0), QVariant(alt.active()));
		m_model->setData(m_model->index(row, 1), QVariant(alt.position() + cross.leftShift()));
		m_model->setData(m_model->index(row, 2), QVariant(alt.position()));
		m_model->setData(m_model->index(row, 3), QVariant(alt.height()));

		QStandardItem* item = m_model->item(row, 1);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable) & (~ Qt::ItemIsEnabled));
		m_model->setItem(row, 1, item);
		ui->tableView->setRowHeight(row, defaultRowHeight);
		++row;
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

void RawDataRiverSurveyCrosssectionWindow::setupView()
{
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);

	RawDataRiverSurveyCrosssectionWindowDelegate* del = new RawDataRiverSurveyCrosssectionWindowDelegate();
	ui->tableView->setItemDelegate(del);

	ui->graphicsView->setParentWindow(this);
	ui->graphicsView->setModel(m_model);
	ui->graphicsView->setSelectionModel(m_selectionModel);
}

void RawDataRiverSurveyCrosssectionWindow::clear()
{
	int rows = m_model->rowCount();
	m_model->removeRows(0, rows);
}

void RawDataRiverSurveyCrosssectionWindow::updateView()
{
	ui->graphicsView->viewport()->update();
}

void RawDataRiverSurveyCrosssectionWindow::cameraFit()
{
	ui->graphicsView->cameraFit();
}

void RawDataRiverSurveyCrosssectionWindow::cameraMoveLeft()
{
	ui->graphicsView->cameraMoveLeft();
}

void RawDataRiverSurveyCrosssectionWindow::cameraMoveRight()
{
	ui->graphicsView->cameraMoveRight();
}

void RawDataRiverSurveyCrosssectionWindow::cameraMoveUp()
{
	ui->graphicsView->cameraMoveUp();
}

void RawDataRiverSurveyCrosssectionWindow::cameraMoveDown()
{
	ui->graphicsView->cameraMoveDown();
}

void RawDataRiverSurveyCrosssectionWindow::cameraZoomIn()
{
	ui->graphicsView->cameraZoomIn();
}

void RawDataRiverSurveyCrosssectionWindow::cameraZoomOut()
{
	ui->graphicsView->cameraZoomOut();
}

void RawDataRiverSurveyCrosssectionWindow::cameraZoomInX()
{
	ui->graphicsView->cameraZoomInX();
}

void RawDataRiverSurveyCrosssectionWindow::cameraZoomOutX()
{
	ui->graphicsView->cameraZoomOutX();
}

void RawDataRiverSurveyCrosssectionWindow::cameraZoomInY()
{
	ui->graphicsView->cameraZoomInY();
}

void RawDataRiverSurveyCrosssectionWindow::cameraZoomOutY()
{
	ui->graphicsView->cameraZoomOutY();
}

void RawDataRiverSurveyCrosssectionWindow::handleDataChange()
{
	if (m_settingUp) {return;}
	RawDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	RawDataRiverCrosssection::AltitudeList before, after;
	before = cross.AltitudeInfo();
	if (! syncData()) { return; }
	after = cross.AltitudeInfo();
	iRICUndoStack::instance().push(new RawDataRiverSurveyCrosssectionEditCommand(false, tr("Edit Elevation Point"), m_editTargetPoint, after, before, this, m_targetRiverSurvey, true));
}

bool RawDataRiverSurveyCrosssectionWindow::syncData()
{
	m_settingUp = true;
	RawDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	RawDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	RawDataRiverCrosssection::AltitudeList old = cross.AltitudeInfo();
	auto it = old.begin();
	// whether activations are valid or not
	QList<int> indices;
	for (int i = 0; i < m_model->rowCount(); ++i) {
		bool active = m_model->data(m_model->index(i, 0)).toBool();
		if (active) { continue; }
		indices.append(i);
	}
	if (! canInactivateSelectedRows(cross, indices)) {
		for (int i = 0; i < m_model->rowCount(); ++i) {
			m_model->setData(m_model->index(i, 0), QVariant((it + i)->active()));
		}
		alist = old;
		m_settingUp = false;
		return false;
	}
	// update the crosssection.
	alist.clear();
	for (int i = 0; i < m_model->rowCount(); ++i) {
		RawDataRiverCrosssection::Altitude alt;
		// active
		alt.setActive(m_model->data(m_model->index(i, 0)).toBool());
		// distance from left bank
		// position
		double oldPos = (it + i)->position();
		double newPos = m_model->data(m_model->index(i, 2)).toDouble();
		if (oldPos != newPos) {
			double min, max;
			if (i == 0) {
				max = (it + i + 1)->position();
				if (newPos > max) { newPos = max; }
			} else if (i == m_model->rowCount() - 1) {
				min = (it + i - 1)->position();
				if (newPos < min) { newPos = min; }
			} else {
				min = (it + i - 1)->position();
				max = (it + i + 1)->position();
				if (newPos > max) {
					newPos = max;
				} else if (newPos < min) {
					newPos = min;
				}
			}
			m_model->setData(m_model->index(i, 1), QVariant(newPos + cross.leftShift()));
			m_model->setData(m_model->index(i, 2), QVariant(newPos));
		}
		alt.setPosition(newPos);
		// height
		alt.setHeight(m_model->data(m_model->index(i, 3)).toDouble());

		alist.push_back(alt);
	}
	updateView();
	m_targetRiverSurvey->updateShapeData();
	m_settingUp = false;
	return true;
}

void RawDataRiverSurveyCrosssectionWindow::crosssectionComboBoxChange(int newindex)
{
	double crosssection = m_crosssectionNames.at(newindex);
	setCrosssection(crosssection);

	informFocusIn();
}

void RawDataRiverSurveyCrosssectionWindow::deleteSelectedRows()
{
	RawDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	RawDataRiverCrosssection::AltitudeList before, after;
	RawDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	before = alist;
	QModelIndexList rows = m_selectionModel->selectedRows();

	// delete
	QList<int> indices;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		indices.append(index.row());
	}
	try {
		cross.removePoint(indices);
	} catch (RawDataRiverCrosssection::ErrorCodes ec) {
		alist = before;
		switch (ec) {
		case RawDataRiverCrosssection::ec_AltitudesMustExistTwo:
			QMessageBox::warning(this, tr("Warning"), tr("At least, two points should be active."));
			break;
		case RawDataRiverCrosssection::ec_FixDelete:
			QMessageBox::warning(this, tr("Warning"), tr("Fixed points and end points cannot be deleted."));
			break;
		case RawDataRiverCrosssection::ec_AltitudesBiased:
			QMessageBox::warning(this, tr("Warning"), tr("There should be more than one point in the right bank side and the left bank side."));
			break;
		default:
			break;
		}
		return;
	}
	after = alist;
	iRICUndoStack::instance().push(new RawDataRiverSurveyCrosssectionEditCommand(false, tr("Delete Elevation Points"), m_editTargetPoint, after, before, this, m_targetRiverSurvey));
	m_selectionModel->clear();
}

void RawDataRiverSurveyCrosssectionWindow::inactivateByWEOnlyThis()
{
	if (! m_editTargetPoint->waterSurfaceElevationSpecified()) {return;}
	QList<int> indices = m_editTargetPoint->getPointsToInactivateUsingWaterElevation();

	RawDataRiverCrosssection::AltitudeList before, after;
	RawDataRiverCrosssection::AltitudeList& alist = m_editTargetPoint->crosssection().AltitudeInfo();
	before = alist;
	if (! canInactivateSelectedRows(m_editTargetPoint->crosssection(), indices)) {
		alist = before;
		return;
	}
	after = alist;
	iRICUndoStack::instance().push(new RawDataRiverSurveyCrosssectionEditCommand(false, tr("Inactivate Elevation Points using water elevation"), m_editTargetPoint, after, before, this, m_targetRiverSurvey));
}

void RawDataRiverSurveyCrosssectionWindow::inactivateByWEAll()
{
	QUndoCommand* group = new QUndoCommand(tr("Inactivate Elevation Points using water elevation"));

	RawDataRiverPathPoint* p = m_targetRiverSurvey->headPoint()->nextPoint();
	bool exec = false;
	while (p != nullptr) {
		if (! p->waterSurfaceElevationSpecified()) {
			p = p->nextPoint();
			continue;
		}
		QList<int> indices = p->getPointsToInactivateUsingWaterElevation();
		RawDataRiverCrosssection::AltitudeList before, after;
		RawDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		before = alist;
		if (! canInactivateSelectedRows(p->crosssection(), indices)) {
			alist = before;
		}
		after = alist;
		new RawDataRiverSurveyCrosssectionEditCommand(false, tr("Inactivate Elevation Points using water elevation"), p, after, before, this, m_targetRiverSurvey, true, group);
		exec = true;
		p = p->nextPoint();
	}
	if (exec) {
		iRICUndoStack::instance().push(group);
	} else {
		delete group;
	}
}

void RawDataRiverSurveyCrosssectionWindow::updateActionStatus()
{
	QModelIndexList rows = m_selectionModel->selectedRows();
	m_deleteAction->setEnabled(rows.count() > 0);
}


RawDataRiverSurveyCrosssectionEditCommand::RawDataRiverSurveyCrosssectionEditCommand(bool apply, const QString& title, RawDataRiverPathPoint* p, const RawDataRiverCrosssection::AltitudeList& after, const RawDataRiverCrosssection::AltitudeList& before, RawDataRiverSurveyCrosssectionWindow* w, RawDataRiverSurvey* rs, bool tableaction, QUndoCommand* parentcommand)
	: QUndoCommand(title, parentcommand)
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

void RawDataRiverSurveyCrosssectionEditCommand::redo()
{
	m_point->crosssection().AltitudeInfo() = m_after;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (m_apply || (m_tableaction && m_first)) {
		m_window->updateView();
	} else {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_window->groupDataItem()->updateCrossectionWindows();
		m_rs->m_mapped = false;
	}
	m_first = false;
}

void RawDataRiverSurveyCrosssectionEditCommand::undo()
{
	m_point->crosssection().AltitudeInfo() = m_before;
	m_point->updateXSecInterpolators();
	m_point->updateRiverShapeInterpolators();
	if (! m_apply) {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_window->groupDataItem()->updateCrossectionWindows();
	}
}

QTableView* RawDataRiverSurveyCrosssectionWindow::tableView()
{
	return ui->tableView;
}

bool RawDataRiverSurveyCrosssectionWindow::canInactivateSelectedRows(RawDataRiverCrosssection& cross, QList<int> indices)
{
	try {
		cross.activate(indices, false);
	} catch (RawDataRiverCrosssection::ErrorCodes ec) {
		switch (ec) {
		case RawDataRiverCrosssection::ec_FixInactivate:
			QMessageBox::warning(this, tr("Warning"), tr("Fixed points and end points cannot be inactivated."));
			break;
		case RawDataRiverCrosssection::ec_AltitudesMustExistTwo:
			// error throwed. Less than two points are active.
			QMessageBox::warning(this, tr("Warning"), tr("At least, two points should be active."));
			break;
		case RawDataRiverCrosssection::ec_AltitudesBiased:
			QMessageBox::warning(this, tr("Warning"), tr("There should be more than one point in the right bank side and the left bank side."));
			break;
		default:
			break;
		}
		return false;
	}
	return true;
}

void RawDataRiverSurveyCrosssectionWindow::informFocusIn()
{
//	m_riverSurvey->setColoredPoints(m_editTargetPoint, m_redLinePoint, m_blueLinePoint);
}

void RawDataRiverSurveyCrosssectionWindow::toggleGridCreatingMode(bool gridMode, RawDataRiverSurvey* rs)
{
	if (gridMode) {
		m_gridCreatingConditionRiverSurvey = rs;
	} else {
		m_gridCreatingConditionRiverSurvey = nullptr;
	}
	ui->surveysTableWidget->setDisabled(gridMode);
	updateRiverPathPoints();
	updateEditTargetPoint();
	setupData();
	ui->graphicsView->toggleGridCreatingMode(gridMode);
}

void RawDataRiverSurveyCrosssectionWindow::update()
{
	ui->graphicsView->viewport()->update();
	QWidget::update();
}

QPixmap RawDataRiverSurveyCrosssectionWindow::snapshot()
{
	RawDataRiverSurveyCrosssectionWindowGraphicsView* g = ui->graphicsView;
	QPixmap pixmap(g->size());
	QPainter painter;
	painter.begin(&pixmap);
	g->render(&painter);
	painter.end();
	return pixmap;
}

QList<QMenu*> RawDataRiverSurveyCrosssectionWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(m_elevationPointMenu);
	return menus;
}

void RawDataRiverSurveyCrosssectionWindow::weCheckboxChange(bool checked)
{
	ui->weSpinBox->setEnabled(checked);
	if (! checked) {
		m_editTargetPoint->clearWaterSurfaceElevation();
	} else {
		m_editTargetPoint->setWaterSurfaceElevation(ui->weSpinBox->value());
	}
	update();
}

void RawDataRiverSurveyCrosssectionWindow::weValueChange(double val)
{
	m_editTargetPoint->setWaterSurfaceElevation(val);
	update();
}

void RawDataRiverSurveyCrosssectionWindow::updateRiverSurveys()
{
	QMap <RawDataRiverSurvey*, bool> enableMap;
	QMap <RawDataRiverSurvey*, QColor > colorMap;

	for (int i = 0; i < m_riverSurveys.count(); ++i) {
		enableMap.insert(m_riverSurveys.at(i), m_riverSurveyEnables.at(i));
		colorMap.insert(m_riverSurveys.at(i), m_riverSurveyColors.at(i));
	}
	m_riverSurveyEnables.clear();
	m_riverSurveys.clear();
	m_riverSurveyColors.clear();
	QList<GraphicsWindowDataItem*> items = m_groupDataItem->childItems();
	for (int i = 0; i < items.count(); ++i) {
		PreProcessorRawdataDataItemInterface* item = dynamic_cast<PreProcessorRawdataDataItemInterface*>(items.at(i));
		RawDataRiverSurvey* rawdata = dynamic_cast<RawDataRiverSurvey*>(item->rawData());
		if (rawdata != nullptr) {
			m_riverSurveys.append(rawdata);
			m_riverSurveyEnables.append(enableMap.value(rawdata, true));
			m_riverSurveyColors.append(colorMap.value(rawdata, m_colorSource->getColor(i)));
		}
	}
	int index = m_riverSurveys.indexOf(m_targetRiverSurvey);
	if (index == -1) {
		if (m_riverSurveys.count() == 0) {
			m_targetRiverSurvey = nullptr;
		} else {
			m_targetRiverSurvey = m_riverSurveys[0];
		}
	}
	updateRiverPathPoints();
	updateSurveysTable();
	updateComboBoxes();
}

void RawDataRiverSurveyCrosssectionWindow::setupSurveyTable()
{
	ui->surveysTableWidget->setColumnCount(3);
	QList<QString> labels;
	labels.append("");
	labels.append(tr("River Survey Data"));
	labels.append(tr("Color"));
	ui->surveysTableWidget->setHorizontalHeaderLabels(labels);
	ui->surveysTableWidget->setColumnWidth(0, TABLE_CHECK_WIDTH);
	ui->surveysTableWidget->setColumnWidth(1, TABLE_DATA_WIDTH);
	ui->surveysTableWidget->setColumnWidth(2, TABLE_COLOR_WIDTH);
	ui->surveysTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->surveysTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	ui->surveysTableWidget->setItemDelegate(new RawDataRiverSurveyCrosssectionWindowTableDelegate());
	ui->surveysTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void RawDataRiverSurveyCrosssectionWindow::updateSurveysTable()
{
	ui->surveysTableWidget->setRowCount(m_riverSurveys.count());
	for (int i = 0; i < m_riverSurveys.count(); ++i) {
		QTableWidgetItem* item;

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, m_riverSurveyEnables.at(i));
		ui->surveysTableWidget->setItem(i, 0, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, m_riverSurveys.at(i)->caption());
		ui->surveysTableWidget->setItem(i, 1, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, m_riverSurveyColors.at(i));
		item->setData(Qt::BackgroundRole, m_riverSurveyColors.at(i));
		ui->surveysTableWidget->setItem(i, 2, item);
		ui->surveysTableWidget->setRowHeight(i, TABLE_ROWHEIGHT);
	}
	int index = m_riverSurveys.indexOf(m_targetRiverSurvey);
	if (index != -1) {
		ui->surveysTableWidget->selectRow(index);
	}
}

void RawDataRiverSurveyCrosssectionWindow::handleSurveyTableItemEdit(QTableWidgetItem* item)
{
	if (item->column() == 0) {
		m_riverSurveyEnables[item->row()] = item->data(Qt::DisplayRole).toBool();
	} else if (item->column() == 2) {
		m_riverSurveyColors[item->row()] = item->data(Qt::DisplayRole).value<QColor>();
	}
}

void RawDataRiverSurveyCrosssectionWindow::handleSurveyTableItemClick(QTableWidgetItem* item)
{
	switch (item->column()) {
	case 0: {
			bool checked = item->data(Qt::DisplayRole).toBool();
			checked = ! checked;
			item->setData(Qt::DisplayRole, checked);
			updateEditTargetPoint();
		}
		break;
	case 2: {
			QColor col = item->data(Qt::DisplayRole).value<QColor>();
			QColor newcolor = QColorDialog::getColor(col, this);
			if (! newcolor.isValid()) {return;}
			item->setData(Qt::DisplayRole, newcolor);
			update();
		}
		break;
	}

	if (item->column() != 2) {return;}
}

void RawDataRiverSurveyCrosssectionWindow::handleSurveyTablecurrentCellChange(int currentRow, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
	if (currentRow == -1) {return;}
	m_targetRiverSurvey = m_riverSurveys[currentRow];
	updateEditTargetPoint();
}

void RawDataRiverSurveyCrosssectionWindow::updateEditTargetPoint()
{
	int index = m_riverSurveys.indexOf(m_targetRiverSurvey);
	if (index == -1) {
		m_editTargetPoint = nullptr;
	} else if (! m_riverSurveyEnables[index]) {
		m_editTargetPoint = nullptr;
	} else if (m_gridCreatingConditionRiverSurvey != nullptr) {
		m_editTargetPoint = nullptr;
	} else {
		m_editTargetPoint = m_riverPathPoints[index];
	}

	RawDataRiverSurveyCrosssectionWindowDelegate* del =
		dynamic_cast<RawDataRiverSurveyCrosssectionWindowDelegate*>(ui->tableView->itemDelegate());
	if (m_editTargetPoint == nullptr) {
		del->setCrosssection(nullptr);
	} else {
		del->setCrosssection(&(m_editTargetPoint->crosssection()));
	}
	setupData();
	updateView();

	// set watersurface elevation
	ui->weCheckBox->blockSignals(true);
	ui->weSpinBox->blockSignals(true);
	if (m_editTargetPoint == nullptr) {
		ui->weCheckBox->setEnabled(false);
		ui->weCheckBox->setChecked(false);
		ui->weSpinBox->setEnabled(false);
	} else {
		ui->weCheckBox->setEnabled(true);
		ui->weCheckBox->setChecked(m_editTargetPoint->waterSurfaceElevationSpecified());
		ui->weSpinBox->setEnabled(m_editTargetPoint->waterSurfaceElevationSpecified());
		ui->weSpinBox->setValue(m_editTargetPoint->waterSurfaceElevationValue());
	}
	ui->weCheckBox->blockSignals(false);
	ui->weSpinBox->blockSignals(false);
}

void RawDataRiverSurveyCrosssectionWindow::updateRiverPathPoints()
{
	m_riverPathPoints.clear();
	for (int i = 0; i < m_riverSurveys.count(); ++i) {
		RawDataRiverPathPoint* p = m_riverSurveys.at(i)->headPoint();
		p = p->nextPoint();
		while (p != nullptr && p->name().toDouble() != m_crosssectionName) {
			p = p->nextPoint();
		}
		m_riverPathPoints.append(p);
	}
	if (m_gridCreatingConditionRiverSurvey != nullptr) {
		RawDataRiverPathPoint* p = m_gridCreatingConditionRiverSurvey->headPoint();
		p = p->nextPoint();
		while (p != nullptr && p->name().toDouble() != m_crosssectionName) {
			p = p->nextPoint();
		}
		m_gridCreatingConditionPoint = p;
	} else {
		m_gridCreatingConditionPoint = nullptr;
	}
}
