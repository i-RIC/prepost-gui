#include "ui_geodatariversurveycrosssectionwindow.h"

#include "geodatarivercrosssection.h"
#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectioneditfrompointdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowprojectdataitem.h"
#include "private/geodatariversurveycrosssectionwindow_datatabledelegate.h"
#include "private/geodatariversurveycrosssectionwindow_impl.h"
#include "private/geodatariversurvey_editcrosssectioncommand.h"
#include "private/geodatariversurveycrosssectionwindow_riversurveytabledelegate.h"
#include "private/geodatariversurveycrosssectionwindow_wsetabledelegate.h"

#include <guibase/widget/realnumbereditwidget.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorhydraulicdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorhydraulicdatagroupdataiteminterface.h>
#include <guicore/pre/hydraulicdata/hydraulicdata.h>
#include <guicore/project/colorsource.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevation.h>
#include <misc/iricundostack.h>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QStandardItemModel>

#include <set>
#include <map>

namespace {

QList<QString> setupCrosssectionNames(const QList<GeoDataRiverSurvey*>& surveys)
{
	std::set<QString> nameSet;
	std::multimap<double, QString> nameMap;
	bool all_numerical = true;

	for (GeoDataRiverSurvey* survey : surveys) {
		GeoDataRiverPathPoint* p = survey->headPoint()->nextPoint();
		while (p != nullptr) {
			if (nameSet.find(p->name()) == nameSet.end()) {
				nameSet.insert(p->name());
				bool numerical;
				double realName = p->name().toDouble(&numerical);
				all_numerical = all_numerical && numerical;
				nameMap.insert({realName, p->name()});
			}
			p = p->nextPoint();
		}
	}

	QList<QString> names;
	if (all_numerical) {
		for (const auto& pair : nameMap) {
			names.push_back(pair.second);
		}
	} else {
		for (const QString& name : nameSet) {
			names.push_back(name);
		}
	}
	return names;
}

} // namespace

GeoDataRiverSurveyCrosssectionWindow::Impl::Impl(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi) :
	m_targetRiverSurvey {nullptr},
	m_editTargetPoint {nullptr},
	m_gridCreatingConditionRiverSurvey {nullptr},
	m_gridCreatingConditionPoint {nullptr},
	m_projectDataItem {pdi},
	m_icon {":/libs/geodata/riversurvey/images/iconRiverCrosssection.png"},
	m_colorSource {new ColorSource {nullptr}},
	m_settingUp {false}
{
	m_colorSource->load(":/libs/geodata/riversurvey/data/colorsource_rs.xml");
}


GeoDataRiverSurveyCrosssectionWindow::Impl::~Impl()
{
	delete m_colorSource;
}

GeoDataRiverSurveyCrosssectionWindow::GeoDataRiverSurveyCrosssectionWindow(PreProcessorGeoDataGroupDataItemInterface* gitem, GeoDataRiverSurveyCrosssectionWindowProjectDataItem* pdi, QWidget* parent) :
	QMainWindow {parent},
	ui {new Ui::GeoDataRiverSurveyCrosssectionWindow},
	impl {new Impl(pdi)}
{
	impl->m_groupDataItem = gitem;

	ui->setupUi(this);
	QList<int> hSizes;
	hSizes.append(600);
	hSizes.append(200);
	ui->hSplitter->setSizes(hSizes);
	QList<int> vSizes;
	vSizes.append(150);
	vSizes.append(150);
	vSizes.append(600);
	ui->vSplitter->setSizes(vSizes);

	setupActions();
	setupToolBars();
	setupModel();
	setupView();
	setupMenu();
	setupSurveyTable();
	setupWaterSurfaceElevationTable();
	updateRiverSurveys();

	connect(impl->m_model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(handleDataChange()));
	connect(ui->surveysTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleSurveyTableItemClick(QTableWidgetItem*)));
	connect(ui->surveysTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleSurveyTableItemEdit(QTableWidgetItem*)));
	connect(ui->surveysTableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(handleSurveyTablecurrentCellChange(int,int,int,int)));
	connect(ui->wsesTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(handleWseTableItemClick(QTableWidgetItem*)));
	connect(ui->wsesTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(handleWseTableItemEdit(QTableWidgetItem*)));
	connect(ui->graphicsView, SIGNAL(drawnRegionChanged()), this, SLOT(handleDrawnRegionChanged()));
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
	impl->m_editFromSelectedPointAction = new QAction(tr("&Edit cross section from the selected point"), this);
	impl->m_editFromSelectedPointWithDialogAction = new QAction(tr("&Edit from Dialog..."), this);
	impl->m_deleteAction = new QAction(tr("&Delete"), this);

	connect(impl->m_editFromSelectedPointAction, SIGNAL(triggered()), this, SLOT(editFromSelectedPoint()));
	connect(impl->m_editFromSelectedPointWithDialogAction, SIGNAL(triggered()), this, SLOT(editFromSelectedPointWithDialog()));
	connect(impl->m_inactivateByWEOnlyThisAction, SIGNAL(triggered()), this, SLOT(inactivateByWEOnlyThis()));
	connect(impl->m_inactivateByWEAllAction, SIGNAL(triggered()), this, SLOT(inactivateByWEAll()));
	connect(impl->m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteSelectedRows()));
}

void GeoDataRiverSurveyCrosssectionWindow::setupMenu()
{
	impl->m_elevationPointMenu = new QMenu(tr("&Elevation Point"));
	auto gview = ui->graphicsView;
	impl->m_elevationPointMenu->addAction(gview->activateAction());
	impl->m_elevationPointMenu->addAction(gview->inactivateAction());

	impl->m_elevationPointMenu->addSeparator();
	QMenu* submenu = impl->m_elevationPointMenu->addMenu(tr("Inactivate using &water elevation"));
	submenu->addAction(impl->m_inactivateByWEOnlyThisAction);
	submenu->addAction(impl->m_inactivateByWEAllAction);

	impl->m_elevationPointMenu->addSeparator();
	impl->m_elevationPointMenu->addAction(impl->m_editFromSelectedPointWithDialogAction);
	impl->m_elevationPointMenu->addAction(gview->moveAction());
	impl->m_elevationPointMenu->addAction(impl->m_deleteAction);
}

void GeoDataRiverSurveyCrosssectionWindow::setupToolBars()
{
	ui->crossSectionToolBar->setFloatable(false);
	ui->viewToolBar->setFloatable(false);
	ui->displayToolBar->setFloatable(false);

	// crossSectionToolBar
	QLabel* l = nullptr;
	QWidget* spacer = nullptr;
	l = new QLabel(tr("Crosssection: "), ui->crossSectionToolBar);
	ui->crossSectionToolBar->addWidget(l);
	impl->m_crosssectionComboBox = new QComboBox(this);
	impl->m_crosssectionComboBox->setMinimumWidth(100);
	ui->crossSectionToolBar->addWidget(impl->m_crosssectionComboBox);

	spacer = new QWidget(ui->crossSectionToolBar);
	spacer->setFixedWidth(10);
	ui->crossSectionToolBar->addWidget(spacer);

	impl->m_referenceCheckBox = new QCheckBox(this);
	impl->m_referenceCheckBox->setText(tr("Reference"));
	impl->m_referenceCheckBox->setChecked(false);
	ui->crossSectionToolBar->addWidget(impl->m_referenceCheckBox);

	impl->m_referenceComboBox = new QComboBox(this);
	impl->m_referenceComboBox->setMinimumWidth(100);
	impl->m_referenceComboBox->setEnabled(false);
	ui->crossSectionToolBar->addWidget(impl->m_referenceComboBox);

	// viewToolBar
	impl->m_autoRescaleCheckBox = new QCheckBox(this);
	impl->m_autoRescaleCheckBox->setText(tr("Auto rescale"));
	impl->m_autoRescaleCheckBox->setChecked(true);
	ui->viewToolBar->addWidget(impl->m_autoRescaleCheckBox);

	spacer = new QWidget(this);
	spacer->setFixedWidth(10);
	ui->viewToolBar->addWidget(spacer);

	l = new QLabel(tr("Aspect ratio: 1 / "), this);
	ui->viewToolBar->addWidget(l);

	impl->m_aspectRatioEdit = new RealNumberEditWidget(this);
	impl->m_aspectRatioEdit->setMaximumWidth(100);
	ui->viewToolBar->addWidget(impl->m_aspectRatioEdit);

	spacer = new QWidget(this);
	spacer->setFixedWidth(10);
	ui->viewToolBar->addWidget(spacer);

	impl->m_fixAspectRatioCheckBox = new QCheckBox(tr("Fix aspect ratio"), this);
	ui->viewToolBar->addWidget(impl->m_fixAspectRatioCheckBox);

	impl->m_fixRegionCheckBox = new QCheckBox(tr("Fix region"), this);
	ui->viewToolBar->addWidget(impl->m_fixRegionCheckBox);

	// displayToolBar
	impl->m_gridDisplayCheckBox = new QCheckBox(this);
	impl->m_gridDisplayCheckBox->setText(tr("Grid"));
	impl->m_gridDisplayCheckBox->setChecked(true);
	ui->displayToolBar->addWidget(impl->m_gridDisplayCheckBox);

	impl->m_scaleDisplayCheckBox = new QCheckBox(this);
	impl->m_scaleDisplayCheckBox->setText(tr("Scale"));
	impl->m_scaleDisplayCheckBox->setChecked(true);
	ui->displayToolBar->addWidget(impl->m_scaleDisplayCheckBox);

	impl->m_markersDisplayCheckBox = new QCheckBox(this);
	impl->m_markersDisplayCheckBox->setText(tr("Left/right bank markers"));
	impl->m_markersDisplayCheckBox->setChecked(true);
	ui->displayToolBar->addWidget(impl->m_markersDisplayCheckBox);

	impl->m_aspectRatioDisplayCheckBox = new QCheckBox(this);
	impl->m_aspectRatioDisplayCheckBox->setText(tr("Aspect ratio"));
	impl->m_aspectRatioDisplayCheckBox->setChecked(true);
	ui->displayToolBar->addWidget(impl->m_aspectRatioDisplayCheckBox);

	auto displaySettingButton = new QPushButton(tr("Display Setting"), this);
	ui->displayToolBar->addWidget(displaySettingButton);

	connect(impl->m_crosssectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(crosssectionComboBoxChange(int)));
	connect(impl->m_referenceCheckBox, SIGNAL(toggled(bool)), impl->m_referenceComboBox, SLOT(setEnabled(bool)));
	connect(impl->m_referenceCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateView()));
	connect(impl->m_referenceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateView()));

	connect(impl->m_aspectRatioEdit, SIGNAL(valueChanged(double)), this, SLOT(handleAspectRatioEdit(double)));
	connect(impl->m_fixAspectRatioCheckBox, SIGNAL(toggled(bool)), this, SLOT(handleFixAspectRatio(bool)));
	connect(impl->m_fixRegionCheckBox, SIGNAL(toggled(bool)), this, SLOT(handleFixRegion(bool)));

	connect(impl->m_gridDisplayCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateView()));
	connect(impl->m_scaleDisplayCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateView()));
	connect(impl->m_aspectRatioDisplayCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateView()));
	connect(impl->m_markersDisplayCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateView()));
	connect(displaySettingButton, SIGNAL(clicked()), this, SLOT(editDisplaySetting()));
}

void GeoDataRiverSurveyCrosssectionWindow::setupModel()
{
	impl->m_model = new QStandardItemModel(0, 4, this);
	impl->m_model->setHeaderData(0, Qt::Horizontal, tr("Active"));
	impl->m_model->setHeaderData(1, Qt::Horizontal, tr("Distance from left bank"));
	impl->m_model->setHeaderData(2, Qt::Horizontal, tr("Position"));
	impl->m_model->setHeaderData(3, Qt::Horizontal, tr("Elevation"));

	impl->m_selectionModel = new QItemSelectionModel(impl->m_model, this);
	connect(impl->m_selectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(updateActionStatus()));
}

void GeoDataRiverSurveyCrosssectionWindow::updateCrossSectionComboBox()
{
	// update crosssectionNames.
	impl->m_crosssectionNames = setupCrosssectionNames(impl->m_riverSurveys);

	impl->m_crosssectionComboBox->blockSignals(true);
	impl->m_crosssectionComboBox->clear();
	for (const QString& name : impl->m_crosssectionNames) {
		impl->m_crosssectionComboBox->addItem(name);
	}
	impl->m_crosssectionComboBox->blockSignals(false);

	int index = impl->m_crosssectionNames.indexOf(impl->m_crosssectionName);
	if (index != - 1) {
		impl->m_crosssectionComboBox->blockSignals(true);
		impl->m_crosssectionComboBox->setCurrentIndex(index);
		impl->m_crosssectionComboBox->blockSignals(false);
		updateEditTargetPoint();
	} else {
		if (impl->m_crosssectionNames.count() != 0) {
			setCrosssection(impl->m_crosssectionNames[0]);
		} else {
			setCrosssection(nullptr);
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindow::updateReferenceComboBox()
{
	GeoDataRiverPathPoint* oldPoint = nullptr;
	if (impl->m_referenceRiverPathPoints.size() > 0 && impl->m_referenceComboBox->currentIndex() >= 0) {
		oldPoint = impl->m_referenceRiverPathPoints.at(impl->m_referenceComboBox->currentIndex()).point;
	}
	int newIndex = 0;

	impl->m_referenceComboBox->clear();
	impl->m_referenceRiverPathPoints.clear();

	int idx = 0;
	for (auto survey : impl->m_riverSurveys) {
		auto point = survey->headPoint()->nextPoint();
		while (point != nullptr) {
			auto name = QString("%1 : %2").arg(survey->caption()).arg(point->name());
			impl->m_referenceComboBox->addItem(name);
			ReferenceRiverPathPoint rp {idx, point};
			impl->m_referenceRiverPathPoints.push_back(rp);
			if (oldPoint == point) {newIndex = impl->m_referenceRiverPathPoints.size() - 1;}
			point = point->nextPoint();
		}
		++ idx;
	}
	impl->m_referenceComboBox->setCurrentIndex(newIndex);
}

void GeoDataRiverSurveyCrosssectionWindow::setRiverSurvey(GeoDataRiverSurvey* rs)
{
	impl->m_targetRiverSurvey = rs;
}

QString GeoDataRiverSurveyCrosssectionWindow::crosssectionName() const
{
	return impl->m_crosssectionName;
}

void GeoDataRiverSurveyCrosssectionWindow::setCrosssection(const QString& crosssection)
{
	impl->m_crosssectionName = crosssection;
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

GeoDataRiverPathPoint* GeoDataRiverSurveyCrosssectionWindow::target() const
{
	return impl->m_editTargetPoint;
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

QAction* GeoDataRiverSurveyCrosssectionWindow::editFromSelectedPointAction() const
{
	return impl->m_editFromSelectedPointAction;
}

QAction* GeoDataRiverSurveyCrosssectionWindow::editFromSelectedPointWithDialogAction() const
{
	return impl->m_editFromSelectedPointWithDialogAction;
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
	// keep selection status
	QItemSelection selection = impl->m_selectionModel->selection();
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

	if (impl->m_editTargetPoint == nullptr) {return;}

	GeoDataRiverCrosssection& cross = impl->m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	int row = 0;

	GeoDataRiverCrosssection::Altitude alt;
	for (auto it = alist.begin(); it != alist.end(); ++it) {
		alt = *it;
		impl->m_model->insertRow(row);
		impl->m_model->setData(impl->m_model->index(row, 0), QVariant(alt.active()));
		impl->m_model->setData(impl->m_model->index(row, 1), QVariant(alt.position() + cross.leftShift()));
		impl->m_model->setData(impl->m_model->index(row, 2), QVariant(alt.position()));
		impl->m_model->setData(impl->m_model->index(row, 3), QVariant(alt.height()));

		QStandardItem* item = impl->m_model->item(row, 1);
		item->setFlags(item->flags() & (~Qt::ItemIsEditable) & (~ Qt::ItemIsEnabled));
		impl->m_model->setItem(row, 1, item);
		ui->tableView->setRowHeight(row, defaultRowHeight);
		++row;
	}
	// connect to table view.
	ui->tableView->setModel(impl->m_model);
	ui->tableView->setSelectionModel(impl->m_selectionModel);
	// restore selection status.
	selection.clear();
	for (int i = 0; i < sel.count(); ++i) {
		SelectionRange r = sel.at(i);
		selection.append(QItemSelectionRange(impl->m_model->index(r.top, r.left), impl->m_model->index(r.bottom, r.right)));
	}
	impl->m_selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Clear);
	// update tableview row heights.
	for (int i = 0; i < impl->m_model->rowCount(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
	impl->m_settingUp = false;
	updateActionStatus();
}

void GeoDataRiverSurveyCrosssectionWindow::setupView()
{
	ui->tableView->setModel(impl->m_model);
	ui->tableView->setSelectionModel(impl->m_selectionModel);

	ui->tableView->setItemDelegate(new DataTableDelegate());

	ui->graphicsView->setParentWindow(this);
	ui->graphicsView->setModel(impl->m_model);
	ui->graphicsView->setSelectionModel(impl->m_selectionModel);
}

void GeoDataRiverSurveyCrosssectionWindow::clear()
{
	int rows = impl->m_model->rowCount();
	impl->m_model->removeRows(0, rows);
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
	GeoDataRiverCrosssection& cross = impl->m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	before = cross.AltitudeInfo();
	if (! syncData()) { return; }
	after = cross.AltitudeInfo();
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Edit Elevation Point"), impl->m_editTargetPoint, after, before, this, impl->m_targetRiverSurvey, true));
}

bool GeoDataRiverSurveyCrosssectionWindow::syncData()
{
	impl->m_settingUp = true;
	GeoDataRiverCrosssection& cross = impl->m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	GeoDataRiverCrosssection::AltitudeList old = cross.AltitudeInfo();
	auto it = old.begin();
	// whether activations are valid or not
	std::vector<int> indices;
	for (int i = 0; i < impl->m_model->rowCount(); ++i) {
		bool active = impl->m_model->data(impl->m_model->index(i, 0)).toBool();
		if (active) { continue; }
		indices.push_back(i);
	}
	if (! canInactivateSelectedRows(cross, indices)) {
		for (int i = 0; i < impl->m_model->rowCount(); ++i) {
			impl->m_model->setData(impl->m_model->index(i, 0), QVariant((it + i)->active()));
		}
		alist = old;
		impl->m_settingUp = false;
		return false;
	}
	// update the crosssection.
	alist.clear();
	for (int i = 0; i < impl->m_model->rowCount(); ++i) {
		GeoDataRiverCrosssection::Altitude alt;
		// active
		alt.setActive(impl->m_model->data(impl->m_model->index(i, 0)).toBool());
		// distance from left bank
		// position
		double oldPos = (it + i)->position();
		double newPos = impl->m_model->data(impl->m_model->index(i, 2)).toDouble();
		if (oldPos != newPos) {
			double min, max;
			if (i == 0) {
				max = (it + i + 1)->position();
				if (newPos > max) { newPos = max; }
			} else if (i == impl->m_model->rowCount() - 1) {
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
			impl->m_model->setData(impl->m_model->index(i, 1), QVariant(newPos + cross.leftShift()));
			impl->m_model->setData(impl->m_model->index(i, 2), QVariant(newPos));
		}
		alt.setPosition(newPos);
		// height
		alt.setHeight(impl->m_model->data(impl->m_model->index(i, 3)).toDouble());

		alist.push_back(alt);
	}
	updateView();
	impl->m_targetRiverSurvey->updateShapeData();
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
	GeoDataRiverCrosssection& cross = impl->m_editTargetPoint->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	before = alist;
	QModelIndexList rows = impl->m_selectionModel->selectedRows();

	// delete
	std::vector<int> indices;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		indices.push_back(index.row());
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
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Delete Elevation Points"), impl->m_editTargetPoint, after, before, this, impl->m_targetRiverSurvey));
	impl->m_selectionModel->clear();
}

void GeoDataRiverSurveyCrosssectionWindow::editFromSelectedPoint()
{
	QModelIndexList rows = impl->m_selectionModel->selectedRows();
	if (rows.count() != 1) {
		QMessageBox::information(this, tr("Information"), tr("To use this function, please select only one point."));
		return;
	}
	ui->graphicsView->enterEditCrosssectionMode();
}

void GeoDataRiverSurveyCrosssectionWindow::editFromSelectedPointWithDialog()
{
	QModelIndexList rows = impl->m_selectionModel->selectedRows();
	int row = rows.at(0).row();

	ui->graphicsView->informModelessDialogOpen();
	auto dialog = new GeoDataRiverSurveyCrossSectionEditFromPointDialog(row, impl->m_editTargetPoint, impl->m_targetRiverSurvey, this, this);
	connect(dialog, SIGNAL(destroyed(QObject*)), ui->graphicsView, SLOT(informModelessDialogClose()));
	dialog->show();
}

void GeoDataRiverSurveyCrosssectionWindow::inactivateByWEOnlyThis()
{
	int index;
	bool selected = selectWSEIndex(&index);
	if (! selected) {return;}

	auto we = waterElevation(index);

	auto indices = impl->m_editTargetPoint->getPointsToInactivateUsingWaterElevation(we);

	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = impl->m_editTargetPoint->crosssection().AltitudeInfo();
	before = alist;
	if (! canInactivateSelectedRows(impl->m_editTargetPoint->crosssection(), indices)) {
		alist = before;
		return;
	}
	after = alist;
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points using water elevation"), impl->m_editTargetPoint, after, before, this, impl->m_targetRiverSurvey));
}

void GeoDataRiverSurveyCrosssectionWindow::inactivateByWEAll()
{
	int index;
	bool selected = selectWSEIndex(&index);
	if (! selected) {return;}

	auto we = waterElevation(index);

	QUndoCommand* group = new QUndoCommand(tr("Inactivate Elevation Points using water elevation"));

	GeoDataRiverPathPoint* p = impl->m_targetRiverSurvey->headPoint()->nextPoint();
	bool exec = false;
	while (p != nullptr) {
		auto indices = p->getPointsToInactivateUsingWaterElevation(we);
		GeoDataRiverCrosssection::AltitudeList before, after;
		GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		before = alist;
		if (! canInactivateSelectedRows(p->crosssection(), indices)) {
			alist = before;
		}
		after = alist;
		new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points using water elevation"), p, after, before, this, impl->m_targetRiverSurvey, true, group);
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
	QModelIndexList rows = impl->m_selectionModel->selectedRows();
	impl->m_editFromSelectedPointWithDialogAction->setEnabled(rows.count() > 0);
	impl->m_deleteAction->setEnabled(rows.count() > 0);
}

QTableView* GeoDataRiverSurveyCrosssectionWindow::tableView()
{
	return ui->tableView;
}

bool GeoDataRiverSurveyCrosssectionWindow::canInactivateSelectedRows(GeoDataRiverCrosssection& cross, const std::vector<int>& indices)
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
	impl->m_targetRiverSurvey->setColoredPoints(impl->m_editTargetPoint, nullptr, nullptr);
}

void GeoDataRiverSurveyCrosssectionWindow::toggleGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs)
{
	if (gridMode) {
		impl->m_gridCreatingConditionRiverSurvey = rs;
	} else {
		impl->m_gridCreatingConditionRiverSurvey = nullptr;
	}
	ui->wsesTableWidget->setDisabled(gridMode);
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
	return impl->m_groupDataItem;
}

void GeoDataRiverSurveyCrosssectionWindow::setSelectedRow(int row)
{
	QItemSelection sel;
	auto topLeft = impl->m_model->index(row, 0);
	auto bottomRight = impl->m_model->index(row, 3);
	sel.select(topLeft, bottomRight);
	impl->m_selectionModel->select(sel, QItemSelectionModel::ClearAndSelect);
}

bool GeoDataRiverSurveyCrosssectionWindow::isAspectRatioFixed() const
{
	return impl->m_fixAspectRatioCheckBox->isChecked();
}

bool GeoDataRiverSurveyCrosssectionWindow::isRegionFixed() const
{
	return impl->m_fixRegionCheckBox->isChecked();
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
	const auto& items = impl->m_groupDataItem->childItems();
	std::vector<GeoDataRiverSurvey*> rivDatas;
	for (int i = 0; i < items.size(); ++i) {
		PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(items.at(i));
		GeoDataRiverSurvey* geodata = dynamic_cast<GeoDataRiverSurvey*>(item->geoData());
		if (geodata == nullptr) {continue;}

		rivDatas.push_back(geodata);
	}
	for (int i = 0; i < rivDatas.size(); ++i) {
		GeoDataRiverSurvey* geodata = rivDatas[i];
		impl->m_riverSurveys.append(geodata);
		impl->m_riverSurveyEnables.append(enableMap.value(geodata, true));
		impl->m_riverSurveyColors.append(colorMap.value(geodata, impl->m_colorSource->getColor(static_cast<int>(rivDatas.size()) - i)));
	}

	if (impl->m_riverSurveys.count() == 0) {
		impl->m_projectDataItem->requestWindowClose();
		return;
	}

	int index = impl->m_riverSurveys.indexOf(impl->m_targetRiverSurvey);
	if (index == -1) {
		impl->m_targetRiverSurvey = impl->m_riverSurveys[0];
	}
	updateRiverPathPoints();
	updateSurveysTable();
	updateCrossSectionComboBox();
	updateReferenceComboBox();
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

	ui->surveysTableWidget->setItemDelegate(new RiverSurveyTableDelegate());
}

void GeoDataRiverSurveyCrosssectionWindow::setupWaterSurfaceElevationTable()
{
	auto w = ui->wsesTableWidget;
	w->setItemDelegate(new WseTableDelegate());

	connect(w, SIGNAL(moveUpRequested(int)), this, SLOT(moveUpWse(int)));
	connect(w, SIGNAL(moveDownRequested(int)), this, SLOT(moveDownWse(int)));
	connect(w, SIGNAL(deleteRequested(int)), this, SLOT(deleteWse(int)));
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
	int index = impl->m_riverSurveys.indexOf(impl->m_targetRiverSurvey);
	if (index != -1) {
		ui->surveysTableWidget->selectRow(index);
	}
}

void GeoDataRiverSurveyCrosssectionWindow::updateWaterSurfaceElevationTable()
{
	auto weList = waterElevationGroup()->hydraulicDatas();

	ui->wsesTableWidget->blockSignals(true);

	ui->wsesTableWidget->setRowCount(weList.size());
	for (int i = 0; i < weList.size(); ++i) {
		QTableWidgetItem* item = nullptr;
		auto we = dynamic_cast<HydraulicDataRiverSurveyWaterElevation*>(weList.at(i)->hydraulicData());
		auto weItem = we->getItem(crosssectionName());

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, weItem != nullptr && weItem->isSpecified());
		item->setFlags(item->flags() & (~Qt::ItemIsEditable));
		ui->wsesTableWidget->setItem(i, 0, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, we->caption());
		ui->wsesTableWidget->setItem(i, 1, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, weItem ? weItem->value() : 0.);
		ui->wsesTableWidget->setItem(i, 2, item);

		item = new QTableWidgetItem();
		item->setData(Qt::DisplayRole, we->color());
		item->setData(Qt::BackgroundRole, we->color());
		ui->wsesTableWidget->setItem(i, 3, item);

		ui->wsesTableWidget->setRowHeight(i, TABLE_ROWHEIGHT);
	}

	ui->wsesTableWidget->blockSignals(false);
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
}

void GeoDataRiverSurveyCrosssectionWindow::handleWseTableItemEdit(QTableWidgetItem* item)
{
	bool specified = false;
	double val = 0;

	auto we = waterElevation(item->row());
	auto name = impl->m_editTargetPoint->name();
	auto weItem = we->getItem(name);
	if (weItem != nullptr) {
		specified = weItem->isSpecified();
		val = weItem->value();
	}

	if (item->column() == 0) {
		we->pushEditItemCommand(name, item->data(Qt::DisplayRole).toBool(), val);
	} else if (item->column() == 1) {
		we->pushEditCaptionCommand(item->data(Qt::DisplayRole).toString());
	} else if (item->column() == 2) {
		we->pushEditItemCommand(name, specified, item->data(Qt::DisplayRole).toDouble());
	} else if (item->column() == 3) {
		we->pushEditColorCommand(item->data(Qt::DisplayRole).value<QColor>());
	}
}

void GeoDataRiverSurveyCrosssectionWindow::handleWseTableItemClick(QTableWidgetItem* item)
{
	switch (item->column()) {
	case 0: {
			bool checked = item->data(Qt::DisplayRole).toBool();
			checked = ! checked;
			item->setData(Qt::DisplayRole, checked);
			update();
		}
		break;
	case 3: {
			QColor col = item->data(Qt::DisplayRole).value<QColor>();
			QColor newcolor = QColorDialog::getColor(col, this);
			if (! newcolor.isValid()) {return;}
			item->setData(Qt::DisplayRole, newcolor);
			update();
		}
		break;
	}
}

void GeoDataRiverSurveyCrosssectionWindow::handleSurveyTablecurrentCellChange(int currentRow, int /*currentColumn*/, int /*previousRow*/, int /*previousColumn*/)
{
	if (currentRow == -1) {return;}
	impl->m_targetRiverSurvey = impl->m_riverSurveys[currentRow];
	updateEditTargetPoint();
}

void GeoDataRiverSurveyCrosssectionWindow::handleAspectRatioEdit(double ratio)
{
	ui->graphicsView->setAspectRatio(ratio);
}

void GeoDataRiverSurveyCrosssectionWindow::handleFixAspectRatio(bool fix)
{
	impl->m_aspectRatioEdit->setDisabled(fix);
}

void GeoDataRiverSurveyCrosssectionWindow::handleFixRegion(bool fix)
{
	if (fix) {
		impl->m_fixAspectRatioCheckBox->setEnabled(false);
		impl->m_aspectRatioEdit->setEnabled(false);
	} else {
		impl->m_fixAspectRatioCheckBox->setEnabled(true);
		impl->m_aspectRatioEdit->setEnabled(! impl->m_fixAspectRatioCheckBox->isChecked());
	}
}

void GeoDataRiverSurveyCrosssectionWindow::handleDrawnRegionChanged()
{
	impl->m_aspectRatioEdit->setValue(ui->graphicsView->aspectRatio());
}

void GeoDataRiverSurveyCrosssectionWindow::moveUpWse(int index)
{
	auto weGroup = waterElevationGroup();
	weGroup->moveUpItem(index);
}

void GeoDataRiverSurveyCrosssectionWindow::moveDownWse(int index)
{
	auto weGroup = waterElevationGroup();
	weGroup->moveDownItem(index);
}

void GeoDataRiverSurveyCrosssectionWindow::deleteWse(int index)
{
	auto weGroup = waterElevationGroup();
	auto we = waterElevation(index);
	QString name = we->caption();
	int result = QMessageBox::information(this, tr("Delete Water Surface Elevation"), tr("Are you sure you want to delete %1?").arg(name), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	weGroup->deleteItem(index);
}

void GeoDataRiverSurveyCrosssectionWindow::updateEditTargetPoint()
{
	int index = impl->m_riverSurveys.indexOf(impl->m_targetRiverSurvey);
	if (index == -1) {
		impl->m_editTargetPoint = nullptr;
	} else if (! impl->m_riverSurveyEnables[index]) {
		impl->m_editTargetPoint = nullptr;
	} else if (impl->m_gridCreatingConditionRiverSurvey != nullptr) {
		impl->m_editTargetPoint = nullptr;
	} else {
		impl->m_editTargetPoint = impl->m_riverPathPoints[index];
	}

	auto del = dynamic_cast<DataTableDelegate*>(ui->tableView->itemDelegate());
	if (impl->m_editTargetPoint == nullptr) {
		del->setCrosssection(nullptr);
	} else {
		del->setCrosssection(&(impl->m_editTargetPoint->crosssection()));
	}
	setupData();
	updateWaterSurfaceElevationTable();
	updateView();
	informFocusIn();
}

bool GeoDataRiverSurveyCrosssectionWindow::selectWSEIndex(int* index)
{
	auto weGroup = impl->m_targetRiverSurvey->hydraulicDataGroupDataItem("waterelevation");
	auto weList = weGroup->hydraulicDatas();

	if (weList.size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("Water surface elevation data does not exist."));
		return false;
	}
	*index = 0;
	if (weList.size() > 1) {
		QStringList names;
		for (const auto& we : weList) {
			names.push_back(we->hydraulicData()->caption());
		}
		bool ok = false;
		QString name = QInputDialog::getItem(this, tr("Select Water Surface Elevation"), tr("Select Water Surface Elevation to use"), names, 0, false, &ok);
		if (! ok) {return false;}
		*index = names.indexOf(name);
	}
	return true;
}

void GeoDataRiverSurveyCrosssectionWindow::editDisplaySetting()
{
	ui->graphicsView->editDisplaySetting();
}

GeoDataRiverSurvey* GeoDataRiverSurveyCrosssectionWindow::targetRiverSurvey() const
{
	return impl->m_targetRiverSurvey;
}

GeoDataRiverSurvey* GeoDataRiverSurveyCrosssectionWindow::gridCreatingConditionRiverSurvey() const
{
	return impl->m_gridCreatingConditionRiverSurvey;
}

GeoDataRiverPathPoint* GeoDataRiverSurveyCrosssectionWindow::gridCreatingConditionPoint() const
{
	return impl->m_gridCreatingConditionPoint;
}

GeoDataRiverPathPoint* GeoDataRiverSurveyCrosssectionWindow::referenceRiverPathPoint() const
{
	if (! impl->m_referenceCheckBox->isChecked())	{return nullptr;}

	int idx = impl->m_referenceComboBox->currentIndex();
	auto rp = impl->m_referenceRiverPathPoints.at(idx);
	return rp.point;
}

QColor GeoDataRiverSurveyCrosssectionWindow::referenceRiverPathPointColor() const
{
	if (! impl->m_referenceCheckBox->isChecked())	{return Qt::black;}

	int idx = impl->m_referenceComboBox->currentIndex();
	auto rp = impl->m_referenceRiverPathPoints.at(idx);

	QColor color = impl->m_riverSurveyColors.at(rp.dataIndex);
	color.setAlphaF(0.4);
	return color;
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

QItemSelectionModel* GeoDataRiverSurveyCrosssectionWindow::selectionModel() const
{
	return impl->m_selectionModel;
}

void GeoDataRiverSurveyCrosssectionWindow::updateRiverPathPoints()
{
	impl->m_riverPathPoints.clear();
	for (int i = 0; i < impl->m_riverSurveys.count(); ++i) {
		GeoDataRiverPathPoint* p = impl->m_riverSurveys.at(i)->headPoint();
		p = p->nextPoint();
		while (p != nullptr && p->name() != impl->m_crosssectionName) {
			p = p->nextPoint();
		}
		impl->m_riverPathPoints.append(p);
	}
	if (impl->m_gridCreatingConditionRiverSurvey != nullptr) {
		GeoDataRiverPathPoint* p = impl->m_gridCreatingConditionRiverSurvey->headPoint();
		p = p->nextPoint();
		while (p != nullptr && p->name() != impl->m_crosssectionName) {
			p = p->nextPoint();
		}
		impl->m_gridCreatingConditionPoint = p;
	} else {
		impl->m_gridCreatingConditionPoint = nullptr;
	}
}

PreProcessorHydraulicDataGroupDataItemInterface* GeoDataRiverSurveyCrosssectionWindow::waterElevationGroup()
{
	return impl->m_targetRiverSurvey->hydraulicDataGroupDataItem("waterelevation");
}

HydraulicDataRiverSurveyWaterElevation* GeoDataRiverSurveyCrosssectionWindow::waterElevation(int index)
{
	auto weGroup = waterElevationGroup();
	auto weList = weGroup->hydraulicDatas();
	if (index < 0 || index >= weList.size()) {return nullptr;}

	return dynamic_cast<HydraulicDataRiverSurveyWaterElevation*>(weList.at(index)->hydraulicData());
}

