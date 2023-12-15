#include "../public/abstractcrosssectionwindow_graphicsview.h"
#include "../public/abstractcrosssectionwindow_controller.h"
#include "abstractcrosssectionwindow_displaysettingtablecontroller.h"
#include "abstractcrosssectionwindow_edittablecontroller.h"
#include "abstractcrosssectionwindow_impl.h"
#include "ui_abstractcrosssectionwindow.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/widget/realnumbereditwidget.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QDockWidget>
#include <QToolBar>

#include <vtkCellData.h>

namespace {

int SPACER_WIDTH = 10;

QWidget* createSpacer(QWidget* parent)
{
	auto spacer = new QWidget(parent);
	spacer->setFixedWidth(SPACER_WIDTH);
	return spacer;
}

} // namespace

AbstractCrosssectionWindow::Impl::Impl(AbstractCrosssectionWindow* w, QWidget* parent) :
	m_viewToolBar {new QToolBar(AbstractCrosssectionWindow::tr("View Setting Toolbar"), parent)},
	m_displayToolBar {new QToolBar(AbstractCrosssectionWindow::tr("Display Setting ToolBar"), parent)},
	m_displaySettingTableController {nullptr},
	m_editTableController {nullptr},
	m_window {w}
{}

AbstractCrosssectionWindow::Impl::~Impl()
{}

void AbstractCrosssectionWindow::Impl::setupSplitterSizes()
{
	auto w = m_window;

	QList<int> hSizes;
	hSizes.push_back(600);
	hSizes.push_back(200);
	w->ui->hSplitter->setSizes(hSizes);

	QList<int> vSizes;
	vSizes.push_back(150);
	vSizes.push_back(600);
	w->ui->vSplitter->setSizes(vSizes);
}

void AbstractCrosssectionWindow::Impl::setupToolBars()
{
	auto w = m_window;

	// viewToolBar
	auto vt = m_viewToolBar;

	m_autoRescaleCheckBox = new QCheckBox(tr("Auto rescale"), w);
	m_autoRescaleCheckBox->setChecked(true);
	vt->addWidget(m_autoRescaleCheckBox);
	connect(m_autoRescaleCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.enableAutoRescale = checked;});
	connect(m_autoRescaleCheckBox, &QCheckBox::clicked, [=](bool){m_window->updateGraphicsView();});

	vt->addWidget(createSpacer(w));

	m_aspectRatioLabel = new QLabel(tr("Aspect ratio (V/H): 1 / "), w);
	vt->addWidget(m_aspectRatioLabel);

	m_aspectRatioEdit = new RealNumberEditWidget(w);
	m_aspectRatioEdit->setMaximumWidth(100);
	vt->addWidget(m_aspectRatioEdit);
	connect(m_aspectRatioEdit, &RealNumberEditWidget::valueChanged, [=](double value){m_window->ui->graphicsView->setAspectRatio(value);});

	vt->addWidget(createSpacer(w));

	m_fixAspectRatioCheckBox = new QCheckBox(tr("Fix aspect ratio"), w);
	vt->addWidget(m_fixAspectRatioCheckBox);
	connect(m_fixAspectRatioCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.fixAspectRatio = checked;});

	m_fixRegionCheckBox = new QCheckBox(tr("Fix region"), w);
	vt->addWidget(m_fixRegionCheckBox);
	connect(m_fixRegionCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.fixRegion = checked;});

	// displayToolBar
	auto dt = m_displayToolBar;

	m_gridDisplayCheckBox = new QCheckBox(tr("Grid"), w);
	m_gridDisplayCheckBox->setChecked(true);
	dt->addWidget(m_gridDisplayCheckBox);
	connect(m_gridDisplayCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.gridVisible = checked;});
	connect(m_gridDisplayCheckBox, &QCheckBox::clicked, [=](bool){m_window->updateGraphicsView();});

	m_scaleDisplayCheckBox = new QCheckBox(tr("Scale"), w);
	m_scaleDisplayCheckBox->setChecked(true);
	dt->addWidget(m_scaleDisplayCheckBox);
	connect(m_scaleDisplayCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.scaleVisible = checked;});
	connect(m_scaleDisplayCheckBox, &QCheckBox::clicked, [=](bool){m_window->updateGraphicsView();});

	m_markersDisplayCheckBox = new QCheckBox(tr("Left/right bank markers"), w);
	m_markersDisplayCheckBox->setChecked(true);
	dt->addWidget(m_markersDisplayCheckBox);
	connect(m_markersDisplayCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.markersVisible = checked;});
	connect(m_markersDisplayCheckBox, &QCheckBox::clicked, [=](bool){m_window->updateGraphicsView();});

	m_aspectRatioDisplayCheckBox = new QCheckBox(tr("Aspect ratio"), w);
	m_aspectRatioDisplayCheckBox->setChecked(true);
	dt->addWidget(m_aspectRatioDisplayCheckBox);
	connect(m_aspectRatioDisplayCheckBox, &QCheckBox::clicked, [=](bool checked){m_displaySetting.aspectRatioVisible = checked;});
	connect(m_aspectRatioDisplayCheckBox, &QCheckBox::clicked, [=](bool){m_window->updateGraphicsView();});

	m_displaySettingButton = new QPushButton(tr("Display Setting"), w);
	dt->addWidget(m_displaySettingButton);
	connect(m_displaySettingButton, &QPushButton::clicked, [=](bool){m_window->openDisplaySettingDialog();});
}

void AbstractCrosssectionWindow::Impl::setupController(QWidget* parent)
{
	m_controller = new Controller(m_window, parent);

	connect(m_controller, &Controller::snapshotClicked, m_window, &AbstractCrosssectionWindow::saveSnapshots);
	connect(m_controller, &Controller::csvExportClicked, m_window, &AbstractCrosssectionWindow::exportCsvs);
}

void AbstractCrosssectionWindow::Impl::setupDisplaySettingTableController()
{
	m_displaySettingTableController = new DisplaySettingTableController(m_window->ui->settingTable, this);
}

void AbstractCrosssectionWindow::Impl::setupEditTableController()
{
	m_editTableController = new EditTableController(m_window->ui->editTable, this);
}

void AbstractCrosssectionWindow::Impl::setupDisplaySettings()
{
	setupDisplaySettings(m_window->grid(), "");
	setupDisplaySettings(m_window->additionalGrid(), m_window->additionalGridPrefix());

	for (auto& s : m_displaySettings) {
		m_displaySettingMap.insert({s.attributeName(), &s});
	}
}

void AbstractCrosssectionWindow::Impl::setupDisplaySettings(v4Structured2dGrid* grid, const QString& prefix)
{
	if (grid == nullptr) {return;}

	for (auto attName : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->vtkData()->data()->GetPointData())) {
		GridAttributeDisplaySettingContainer setting(attName, grid->attributeDataProvider(), GridAttributeDisplaySettingContainer::Position::Node, grid, prefix);
		QString name = attName.c_str();
		if (name.toLower().contains("elevation")) {
			setting.visible = true;
			setting.mode = GridAttributeDisplaySettingContainer::Mode::AsElevation;
		}
		if (m_window->preColorMapSetting(attName) == nullptr) {
			setting.usePreColor = false;
		}
		m_displaySettings.push_back(setting);
	}
	for (auto attName : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->vtkData()->data()->GetCellData())) {
		GridAttributeDisplaySettingContainer setting(attName, grid->attributeDataProvider(), GridAttributeDisplaySettingContainer::Position::Cell, grid, prefix);
		if (m_window->preColorMapSetting(attName) == nullptr) {
			setting.usePreColor = false;
		}
		m_displaySettings.push_back(setting);
	}
	for (auto attName : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->vtkIEdgeData()->data()->GetCellData())) {
		GridAttributeDisplaySettingContainer setting(attName, grid->attributeDataProvider(), GridAttributeDisplaySettingContainer::Position::IEdge, grid, prefix);
		if (m_window->preColorMapSetting(attName) == nullptr) {
			setting.usePreColor = false;
		}
		m_displaySettings.push_back(setting);
	}
	for (auto attName : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->vtkJEdgeData()->data()->GetCellData())) {
		GridAttributeDisplaySettingContainer setting(attName, grid->attributeDataProvider(), GridAttributeDisplaySettingContainer::Position::JEdge, grid, prefix);
		if (m_window->preColorMapSetting(attName) == nullptr) {
			setting.usePreColor = false;
		}
		m_displaySettings.push_back(setting);
	}
}

AbstractCrosssectionWindow::GraphicsView* AbstractCrosssectionWindow::Impl::graphicsView() const
{
	return m_window->ui->graphicsView;
}

void AbstractCrosssectionWindow::Impl::updateColorMapValueRanges()
{
	for (auto& s : m_displaySettings) {
		if (s.position() == GridAttributeDisplaySettingContainer::Position::Node) {
			s.colorMapSetting->setAutoValueRange(s.grid->vtkData()->valueRangeSet().pointDataValueRange(s.attributeName()));
		} else if (s.position() == GridAttributeDisplaySettingContainer::Position::Cell) {
			s.colorMapSetting->setAutoValueRange(s.grid->vtkData()->valueRangeSet().cellDataValueRange(s.attributeName()));
		} else if (s.position() == GridAttributeDisplaySettingContainer::Position::IEdge) {
			s.colorMapSetting->setAutoValueRange(s.grid->vtkIEdgeData()->valueRangeSet().cellDataValueRange(s.attributeName()));
		} else if (s.position() == GridAttributeDisplaySettingContainer::Position::JEdge) {
			s.colorMapSetting->setAutoValueRange(s.grid->vtkJEdgeData()->valueRangeSet().cellDataValueRange(s.attributeName()));
		}
	}
}

void AbstractCrosssectionWindow::Impl::updateAspectRatioEdit()
{
	auto ratio = m_window->ui->graphicsView->aspectRatio();
	m_aspectRatioEdit->setValue(ratio);
}

void AbstractCrosssectionWindow::Impl::setTargetDirection(Direction dir)
{
	if (dir == Direction::I) {
		m_markersDisplayCheckBox->setText(tr("Left/right bank markers"));
	} else if (dir == Direction::J) {
		m_markersDisplayCheckBox->setText(tr("Upstream/downstream markers"));
	}
}
