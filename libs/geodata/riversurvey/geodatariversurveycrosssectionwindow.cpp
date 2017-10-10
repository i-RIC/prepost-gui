#include "ui_geodatariversurveycrosssectionwindow.h"

#include "geodatarivercrosssection.h"
#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowdelegate.h"
#include "geodatariversurveycrosssectionwindowprojectdataitem.h"

#include <guibase/widget/centeredcheckbox.h>
#include <guibase/widget/coloreditwidget.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/colorsource.h>
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

class GeoDataRiverSurveyCrosssectionWindowTableDelegate : public QItemDelegate
{
public:
	GeoDataRiverSurveyCrosssectionWindowTableDelegate(QObject* parent = nullptr): QItemDelegate(parent) {}
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

GeoDataRiverSurveyCrosssectionWindow::Impl::Impl(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi) :
	m_gridCreatingConditionRiverSurvey {nullptr},
	m_gridCreatingConditionPoint {nullptr},
	m_projectDataItem {pdi},
	m_icon {":/libs/geodata/riversurvey/images/iconRiverCrosssection.png"},
	m_colorSource {new ColorSource {nullptr}},
	m_settingUp {false}
{}


GeoDataRiverSurveyCrosssectionWindow::Impl::~Impl()
{
	delete m_colorSource;
}

GeoDataRiverSurveyCrosssectionWindow::GeoDataRiverSurveyCrosssectionWindow(PreProcessorGeoDataGroupDataItemInterface* gitem, GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent) :
	QMainWindow {parent},
	ui {new Ui::GeoDataRiverSurveyCrosssectionWindow},
	impl {new Impl(pdi)}
{
	m_groupDataItem = gitem;

	m_targetRiverSurvey = nullptr;
	m_editTargetPoint = nullptr;

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


GeoDataRiverSurveyCrosssectionWindow::~GeoDataRiverSurveyCrosssectionWindow()
{
	impl->m_projectDataItem->informWindowClose();
	delete impl;
	delete ui;
}

void GeoDataRiverSurveyCrosssectionWindow::setupActions()
{
	impl->m_inactivateByWEOnlyThisAction = new QAction(tr("&This cross-section only"), this);
	impl->m_inactivateByWEAllAction = new QAction(tr("All cross-sections"), this);
	impl->m_deleteAction = new QAction(tr("&Delete"), this);

	connect(impl->m_inactivateByWEOnlyThisAction, SIGNAL(triggered()), this, SLOT(inactivateByWEOnlyThis()));
	connect(impl->m_inactivateByWEAllAction, SIGNAL(triggered()), this, SLOT(inactivateByWEAll()));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedRows()));
}

void GeoDataRiverSurveyCrosssectionWindow::setupMenu()
{
	impl->m_elevationPointMenu = new QMenu(tr("&Elevation Point"));
	GeoDataRiverSurveyCrosssectionWindowGraphicsView* gview = ui->graphicsView;
	impl->m_elevationPointMenu->addAction(gview->activateAction());
	impl->m_elevationPointMenu->addAction(gview->inactivateAction());

	impl->m_elevationPointMenu->addSeparator();
	QMenu* submenu = impl->m_elevationPointMenu->addMenu(tr("Inactivate using &water elevation"));
	submenu->addAction(impl->m_inactivateByWEOnlyThisAction);
	submenu->addAction(impl->m_inactivateByWEAllAction);

	impl->m_elevationPointMenu->addSeparator();
	impl->m_elevationPointMenu->addAction(gview->moveAction());
	impl->m_elevationPointMenu->addAction(impl->m_deleteAction);
}

void GeoDataRiverSurveyCrosssectionWindow::setupToolBar()
{
	ui->toolBar->setFloatable(false);

	QLabel* l;
	QWidget* spacer;
	l = new QLabel(tr("Crosssection: "), ui->toolBar);
	ui->toolBar->addWidget(l);
	impl->m_crosssectionComboBox = new QComboBox(this);
	impl->m_crosssectionComboBox->setMinimumWidth(100);
	ui->toolBar->addWidget(impl->m_crosssectionComboBox);

	spacer = new QWidget(ui->toolBar);
	spacer->setFixedWidth(10);
	ui->toolBar->addWidget(spacer);

	impl->m_autoRescaleCheckBox = new QCheckBox(this);
	impl->m_autoRescaleCheckBox->setText(tr("Auto rescale"));
	impl->m_autoRescaleCheckBox->setChecked(true);
	ui->toolBar->addWidget(impl->m_autoRescaleCheckBox);

	ui->weSpinBox->setEnabled(false);

	connect(impl->m_crosssectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(crosssectionComboBoxChange(int)));
	connect(ui->weCheckBox, SIGNAL(toggled(bool)), this, SLOT(weCheckboxChange(bool)));
	connect(ui->weSpinBox, SIGNAL(valueChanged(double)), this, SLOT(weValueChange(double)));
}

void GeoDataRiverSurveyCrosssectionWindow::setupModel()
{
	m_model = new QStandardItemModel(0, 4, this);
	m_model->setHeaderData(0, Qt::Horizontal, tr("Active"));
	m_model->setHeaderData(1, Qt::Horizontal, tr("Distance from left bank"));
	m_model->setHeaderData(2, Qt::Horizontal, tr("Position"));
	m_model->setHeaderData(3, Qt::Horizontal, tr("Elevation"));

	m_selectionModel = new QItemSelectionModel(m_model, this);
	connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(updateActionStatus()));
}

void GeoDataRiverSurveyCrosssectionWindow::updateComboBoxes()
{
	// update crosssectionNames.
	QMap<QString, bool> tmpNames;
	for (int i = 0; i < impl->m_riverSurveys.count(); ++i) {
		GeoDataRiverSurvey* rs = impl->m_riverSurveys.at(i);
		GeoDataRiverPathPoint* p = rs->headPoint();
		p = p->nextPoint();
		while (p != nullptr) {
			tmpNames.insert(p->name(), false);
			p = p->nextPoint();
		}
	}
	impl->m_crosssectionNames = tmpNames.keys();

	impl->m_crosssectionComboBox->blockSignals(true);
	impl->m_crosssectionComboBox->clear();
	for (int i = 0; i < impl->m_crosssectionNames.size(); ++i) {
		QString name = impl->m_crosssectionNames.at(i);
		impl->m_crosssectionComboBox->addItem(name);
	}
	impl->m_crosssectionComboBox->blockSignals(false);

	int index = impl->m_crosssectionNames.indexOf(m_crosssectionName);
	if (index != - 1) {
		impl->m_crosssectionComboBox->blockSignals(true);
		impl->m_crosssectionComboBox->setCurrentIndex(index);
		impl->m_crosssectionComboBox->blockSignals(false);
		updateEditTargetPoint();
	} else {
		if (impl->m_crosssectionNames.count() != 0) {
			setCrosssection(impl->m_crosssectionNames[0]);
		} else {
			setCrosssection(0);
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindow::setRiverSurvey(GeoDataRiverSurvey* rs)
{
	m_targetRiverSurvey = rs;
}

void GeoDataRiverSurveyCrosssectionWindow::setCrosssection(const QString& crosssection)
{
	m_crosssectionName = crosssection;
	updateRiverPathPoints();

	impl->m_crosssectionComboBox->blockSignals(true);
	int index = impl->m_crosssectionNames.indexOf(crosssection);
	if (index != - 1) {
		impl->m_crosssectionComboBox->setCurrentIndex(index);
	}
	impl->m_crosssectionComboBox->blockSignals(false);

	updateEditTargetPoint();

	if (impl->m_autoRescaleCheckBox->isChecked()) {
		cameraFit();
	}
	// set window title.
	setWindowTitle(QString(tr("Crosssection Window : %2").arg(crosssection)));
}

struct SelectionRange {
	int top;
	int left;
	int bottom;
	int right;
};

QAction* GeoDataRiverSurveyCrosssectionWindow::deleteAction() const
{
	return impl->m_deleteAction;
}

QAction* GeoDataRiverSurveyCrosssectionWindow::inactivateByWEOnlyThisAction() const
{
	return impl->m_inactivateByWEOnlyThisAction;
}

QAction* GeoDataRiverSurveyCrosssectionWindow::inactivateByWEAllAction() const
{
	return impl->m_inactivateByWEAllAction;
}

void GeoDataRiverSurveyCrosssectionWindow::setupData()
{
	impl->m_settingUp = true;
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

	GeoDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	int row = 0;

	GeoDataRiverCrosssection::Altitude alt;
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
	impl->m_settingUp = false;
	updateActionStatus();
}

void GeoDataRiverSurveyCrosssectionWindow::setupView()
{
	ui->tableView->setModel(m_model);
	ui->tableView->setSelectionModel(m_selectionModel);

	GeoDataRiverSurveyCrosssectionWindowDelegate* del = new GeoDataRiverSurveyCrosssectionWindowDelegate();
	ui->tableView->setItemDelegate(del);

	ui->graphicsView->setParentWindow(this);
	ui->graphicsView->setModel(m_model);
	ui->graphicsView->setSelectionModel(m_selectionModel);
}

void GeoDataRiverSurveyCrosssectionWindow::clear()
{
	int rows = m_model->rowCount();
	m_model->removeRows(0, rows);
}

void GeoDataRiverSurveyCrosssectionWindow::updateView()
{
	ui->graphicsView->viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraFit()
{
	ui->graphicsView->cameraFit();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraMoveLeft()
{
	ui->graphicsView->cameraMoveLeft();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraMoveRight()
{
	ui->graphicsView->cameraMoveRight();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraMoveUp()
{
	ui->graphicsView->cameraMoveUp();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraMoveDown()
{
	ui->graphicsView->cameraMoveDown();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraZoomIn()
{
	ui->graphicsView->cameraZoomIn();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraZoomOut()
{
	ui->graphicsView->cameraZoomOut();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraZoomInX()
{
	ui->graphicsView->cameraZoomInX();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraZoomOutX()
{
	ui->graphicsView->cameraZoomOutX();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraZoomInY()
{
	ui->graphicsView->cameraZoomInY();
}

void GeoDataRiverSurveyCrosssectionWindow::cameraZoomOutY()
{
	ui->graphicsView->cameraZoomOutY();
}

void GeoDataRiverSurveyCrosssectionWindow::handleDataChange()
{
	if (impl->m_settingUp) {return;}
	GeoDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	before = cross.AltitudeInfo();
	if (! syncData()) { return; }
	after = cross.AltitudeInfo();
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Edit Elevation Point"), m_editTargetPoint, after, before, this, m_targetRiverSurvey, true));
}

bool GeoDataRiverSurveyCrosssectionWindow::syncData()
{
	impl->m_settingUp = true;
	GeoDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	GeoDataRiverCrosssection::AltitudeList old = cross.AltitudeInfo();
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
		impl->m_settingUp = false;
		return false;
	}
	// update the crosssection.
	alist.clear();
	for (int i = 0; i < m_model->rowCount(); ++i) {
		GeoDataRiverCrosssection::Altitude alt;
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
	impl->m_settingUp = false;
	return true;
}

void GeoDataRiverSurveyCrosssectionWindow::crosssectionComboBoxChange(int newindex)
{
	auto crosssection = impl->m_crosssectionNames.at(newindex);
	setCrosssection(crosssection);

	informFocusIn();
}

void GeoDataRiverSurveyCrosssectionWindow::deleteSelectedRows()
{
	GeoDataRiverCrosssection& cross = m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
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
	} catch (GeoDataRiverCrosssection::ErrorCodes ec) {
		alist = before;
		switch (ec) {
		case GeoDataRiverCrosssection::ec_AltitudesMustExistTwo:
			QMessageBox::warning(this, tr("Warning"), tr("At least, two points should be active."));
			break;
		case GeoDataRiverCrosssection::ec_FixDelete:
			QMessageBox::warning(this, tr("Warning"), tr("Fixed points and end points cannot be deleted."));
			break;
		case GeoDataRiverCrosssection::ec_AltitudesBiased:
			QMessageBox::warning(this, tr("Warning"), tr("There should be more than one point in the right bank side and the left bank side."));
			break;
		default:
			break;
		}
		return;
	}
	after = alist;
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Delete Elevation Points"), m_editTargetPoint, after, before, this, m_targetRiverSurvey));
	m_selectionModel->clear();
}

void GeoDataRiverSurveyCrosssectionWindow::inactivateByWEOnlyThis()
{
	if (! m_editTargetPoint->waterSurfaceElevationSpecified()) {return;}
	QList<int> indices = m_editTargetPoint->getPointsToInactivateUsingWaterElevation();

	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = m_editTargetPoint->crosssection().AltitudeInfo();
	before = alist;
	if (! canInactivateSelectedRows(m_editTargetPoint->crosssection(), indices)) {
		alist = before;
		return;
	}
	after = alist;
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points using water elevation"), m_editTargetPoint, after, before, this, m_targetRiverSurvey));
}

void GeoDataRiverSurveyCrosssectionWindow::inactivateByWEAll()
{
	QUndoCommand* group = new QUndoCommand(tr("Inactivate Elevation Points using water elevation"));

	GeoDataRiverPathPoint* p = m_targetRiverSurvey->headPoint()->nextPoint();
	bool exec = false;
	while (p != nullptr) {
		if (! p->waterSurfaceElevationSpecified()) {
			p = p->nextPoint();
			continue;
		}
		QList<int> indices = p->getPointsToInactivateUsingWaterElevation();
		GeoDataRiverCrosssection::AltitudeList before, after;
		GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		before = alist;
		if (! canInactivateSelectedRows(p->crosssection(), indices)) {
			alist = before;
		}
		after = alist;
		new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points using water elevation"), p, after, before, this, m_targetRiverSurvey, true, group);
		exec = true;
		p = p->nextPoint();
	}
	if (exec) {
		iRICUndoStack::instance().push(group);
	} else {
		delete group;
	}
}

void GeoDataRiverSurveyCrosssectionWindow::updateActionStatus()
{
	QModelIndexList rows = m_selectionModel->selectedRows();
	impl->m_deleteAction->setEnabled(rows.count() > 0);
}

GeoDataRiverSurvey::EditCrosssectionCommand::EditCrosssectionCommand(bool apply, const QString& title, GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool tableaction, QUndoCommand* parentcommand)
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

void GeoDataRiverSurvey::EditCrosssectionCommand::redo()
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
		m_rs->setMapped(false);
	}
	m_first = false;
}

void GeoDataRiverSurvey::EditCrosssectionCommand::undo()
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

QTableView* GeoDataRiverSurveyCrosssectionWindow::tableView()
{
	return ui->tableView;
}

bool GeoDataRiverSurveyCrosssectionWindow::canInactivateSelectedRows(GeoDataRiverCrosssection& cross, QList<int> indices)
{
	try {
		cross.activate(indices, false);
	} catch (GeoDataRiverCrosssection::ErrorCodes ec) {
		switch (ec) {
		case GeoDataRiverCrosssection::ec_FixInactivate:
			QMessageBox::warning(this, tr("Warning"), tr("Fixed points and end points cannot be inactivated."));
			break;
		case GeoDataRiverCrosssection::ec_AltitudesMustExistTwo:
			// error throwed. Less than two points are active.
			QMessageBox::warning(this, tr("Warning"), tr("At least, two points should be active."));
			break;
		case GeoDataRiverCrosssection::ec_AltitudesBiased:
			QMessageBox::warning(this, tr("Warning"), tr("There should be more than one point in the right bank side and the left bank side."));
			break;
		default:
			break;
		}
		return false;
	}
	return true;
}

void GeoDataRiverSurveyCrosssectionWindow::informFocusIn()
{
	m_targetRiverSurvey->setColoredPoints(m_editTargetPoint, nullptr, nullptr);
}

void GeoDataRiverSurveyCrosssectionWindow::toggleGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs)
{
	if (gridMode) {
		impl->m_gridCreatingConditionRiverSurvey = rs;
	} else {
		impl->m_gridCreatingConditionRiverSurvey = nullptr;
	}
	ui->surveysTableWidget->setDisabled(gridMode);
	updateRiverPathPoints();
	updateEditTargetPoint();
	setupData();
	ui->graphicsView->toggleGridCreatingMode(gridMode);
}

void GeoDataRiverSurveyCrosssectionWindow::update()
{
	ui->graphicsView->viewport()->update();
	QWidget::update();
}

const QIcon& GeoDataRiverSurveyCrosssectionWindow::icon() const
{
	return impl->m_icon;
}

QPixmap GeoDataRiverSurveyCrosssectionWindow::snapshot()
{
	GeoDataRiverSurveyCrosssectionWindowGraphicsView* g = ui->graphicsView;
	QPixmap pixmap(g->size());
	QPainter painter;
	painter.begin(&pixmap);
	g->render(&painter);
	painter.end();
	return pixmap;
}

QList<QMenu*> GeoDataRiverSurveyCrosssectionWindow::getAdditionalMenus() const
{
	QList<QMenu*> menus;
	menus.append(impl->m_elevationPointMenu);
	return menus;
}

QToolBar* GeoDataRiverSurveyCrosssectionWindow::getAdditionalToolBar() const
{
	return nullptr;
}

PreProcessorGeoDataGroupDataItemInterface* GeoDataRiverSurveyCrosssectionWindow::groupDataItem() const
{
	return m_groupDataItem;
}

void GeoDataRiverSurveyCrosssectionWindow::weCheckboxChange(bool checked)
{
	ui->weSpinBox->setEnabled(checked);
	if (! checked) {
		m_editTargetPoint->clearWaterSurfaceElevation();
	} else {
		m_editTargetPoint->setWaterSurfaceElevation(ui->weSpinBox->value());
	}
	update();
}

void GeoDataRiverSurveyCrosssectionWindow::weValueChange(double val)
{
	m_editTargetPoint->setWaterSurfaceElevation(val);
	update();
}

void GeoDataRiverSurveyCrosssectionWindow::updateRiverSurveys()
{
	QMap <GeoDataRiverSurvey*, bool> enableMap;
	QMap <GeoDataRiverSurvey*, QColor > colorMap;

	for (int i = 0; i < impl->m_riverSurveys.count(); ++i) {
		enableMap.insert(impl->m_riverSurveys.at(i), impl->m_riverSurveyEnables.at(i));
		colorMap.insert(impl->m_riverSurveys.at(i), impl->m_riverSurveyColors.at(i));
	}
	impl->m_riverSurveyEnables.clear();
	impl->m_riverSurveys.clear();
	impl->m_riverSurveyColors.clear();
	const auto& items = m_groupDataItem->childItems();
	for (int i = 0; i < items.size(); ++i) {
		PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(items.at(i));
		GeoDataRiverSurvey* geodata = dynamic_cast<GeoDataRiverSurvey*>(item->geoData());
		if (geodata != nullptr) {
			impl->m_riverSurveys.append(geodata);
			impl->m_riverSurveyEnables.append(enableMap.value(geodata, true));
			impl->m_riverSurveyColors.append(colorMap.value(geodata, impl->m_colorSource->getColor(i)));
		}
	}
	if (impl->m_riverSurveys.count() == 0) {
		impl->m_projectDataItem->requestWindowClose();
		return;
	}

	int index = impl->m_riverSurveys.indexOf(m_targetRiverSurvey);
	if (index == -1) {
		m_targetRiverSurvey = impl->m_riverSurveys[0];
	}
	updateRiverPathPoints();
	updateSurveysTable();
	updateComboBoxes();
}

void GeoDataRiverSurveyCrosssectionWindow::setupSurveyTable()
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

	ui->surveysTableWidget->setItemDelegate(new GeoDataRiverSurveyCrosssectionWindowTableDelegate());
	ui->surveysTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void GeoDataRiverSurveyCrosssectionWindow::updateSurveysTable()
{
	ui->surveysTableWidget->setRowCount(impl->m_riverSurveys.count());
	for (int i = 0; i < impl->m_riverSurveys.count(); ++i) {
		QTableWidgetItem* item;

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, impl->m_riverSurveyEnables.at(i));
		ui->surveysTableWidget->setItem(i, 0, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, impl->m_riverSurveys.at(i)->caption());
		ui->surveysTableWidget->setItem(i, 1, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, impl->m_riverSurveyColors.at(i));
		item->setData(Qt::BackgroundRole, impl->m_riverSurveyColors.at(i));
		ui->surveysTableWidget->setItem(i, 2, item);
		ui->surveysTableWidget->setRowHeight(i, TABLE_ROWHEIGHT);
	}
	int index = impl->m_riverSurveys.indexOf(m_targetRiverSurvey);
	if (index != -1) {
		ui->surveysTableWidget->selectRow(index);
	}
}

void GeoDataRiverSurveyCrosssectionWindow::handleSurveyTableItemEdit(QTableWidgetItem* item)
{
	if (item->column() == 0) {
		impl->m_riverSurveyEnables[item->row()] = item->data(Qt::DisplayRole).toBool();
	} else if (item->column() == 2) {
		impl->m_riverSurveyColors[item->row()] = item->data(Qt::DisplayRole).value<QColor>();
	}
}

void GeoDataRiverSurveyCrosssectionWindow::handleSurveyTableItemClick(QTableWidgetItem* item)
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

void GeoDataRiverSurveyCrosssectionWindow::handleSurveyTablecurrentCellChange(int currentRow, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
	if (currentRow == -1) {return;}
	m_targetRiverSurvey = impl->m_riverSurveys[currentRow];
	updateEditTargetPoint();
}

void GeoDataRiverSurveyCrosssectionWindow::updateEditTargetPoint()
{
	int index = impl->m_riverSurveys.indexOf(m_targetRiverSurvey);
	if (index == -1) {
		m_editTargetPoint = nullptr;
	} else if (! impl->m_riverSurveyEnables[index]) {
		m_editTargetPoint = nullptr;
	} else if (impl->m_gridCreatingConditionRiverSurvey != nullptr) {
		m_editTargetPoint = nullptr;
	} else {
		m_editTargetPoint = impl->m_riverPathPoints[index];
	}

	GeoDataRiverSurveyCrosssectionWindowDelegate* del =
		dynamic_cast<GeoDataRiverSurveyCrosssectionWindowDelegate*>(ui->tableView->itemDelegate());
	if (m_editTargetPoint == nullptr) {
		del->setCrosssection(nullptr);
	} else {
		del->setCrosssection(&(m_editTargetPoint->crosssection()));
	}
	setupData();
	updateView();
	informFocusIn();

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

GeoDataRiverSurvey* GeoDataRiverSurveyCrosssectionWindow::gridCreatingConditionRiverSurvey() const
{
	return impl->m_gridCreatingConditionRiverSurvey;
}

GeoDataRiverPathPoint* GeoDataRiverSurveyCrosssectionWindow::gridCreatingConditionPoint() const
{
	return impl->m_gridCreatingConditionPoint;
}

const QList<bool>& GeoDataRiverSurveyCrosssectionWindow::riverSurveyEnables() const
{
	return impl->m_riverSurveyEnables;
}

const QList<GeoDataRiverPathPoint*>& GeoDataRiverSurveyCrosssectionWindow::riverPathPoints() const
{
	return impl->m_riverPathPoints;
}

const QList<QColor>& GeoDataRiverSurveyCrosssectionWindow::riverSurveyColors() const
{
	return impl->m_riverSurveyColors;
}

void GeoDataRiverSurveyCrosssectionWindow::updateRiverPathPoints()
{
	impl->m_riverPathPoints.clear();
	for (int i = 0; i < impl->m_riverSurveys.count(); ++i) {
		GeoDataRiverPathPoint* p = impl->m_riverSurveys.at(i)->headPoint();
		p = p->nextPoint();
		while (p != nullptr && p->name() != m_crosssectionName) {
			p = p->nextPoint();
		}
		impl->m_riverPathPoints.append(p);
	}
	if (impl->m_gridCreatingConditionRiverSurvey != nullptr) {
		GeoDataRiverPathPoint* p = impl->m_gridCreatingConditionRiverSurvey->headPoint();
		p = p->nextPoint();
		while (p != nullptr && p->name() != m_crosssectionName) {
			p = p->nextPoint();
		}
		impl->m_gridCreatingConditionPoint = p;
	} else {
		impl->m_gridCreatingConditionPoint = nullptr;
	}
}
