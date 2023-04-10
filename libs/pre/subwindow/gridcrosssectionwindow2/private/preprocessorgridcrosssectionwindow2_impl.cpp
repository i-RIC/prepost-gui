#include "../../../datamodel/preprocessorgriddataitem.h"
#include "../../../datamodel/preprocessorgridtypedataitem.h"
#include "preprocessorgridcrosssectionwindow2_controller.h"
#include "preprocessorgridcrosssectionwindow2_displaysettingtablecontroller.h"
#include "preprocessorgridcrosssectionwindow2_edittablecontroller.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"
#include "ui_preprocessorgridcrosssectionwindow2.h"

#include <guibase/widget/realnumbereditwidget.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/grid/structured2dgrid.h>

#include <QDockWidget>

namespace {

int SPACER_WIDTH = 10;

QWidget* createSpacer(QWidget* parent)
{
	auto spacer = new QWidget(parent);
	spacer->setFixedWidth(SPACER_WIDTH);
	return spacer;
}

} // namespace

PreProcessorGridCrosssectionWindow2::Impl::Impl(PreProcessorGridDataItem* item, PreProcessorGridCrosssectionWindow2* w) :
	m_displaySettingTableController {nullptr},
	m_gridDataItem {item},
	m_window {w}
{}

PreProcessorGridCrosssectionWindow2::Impl::~Impl()
{}

void PreProcessorGridCrosssectionWindow2::Impl::setupSplitterSizes()
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

void PreProcessorGridCrosssectionWindow2::Impl::setupToolBars()
{
	auto w = m_window;

	// viewToolBar
	auto vt = w->ui->viewToolBar;

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
	auto dt = w->ui->displayToolBar;

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

void PreProcessorGridCrosssectionWindow2::Impl::setupController()
{
	m_controller = new Controller(this, m_window);

	auto dockWidget = new QDockWidget(tr("Controller"), m_window);
	dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
	dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	dockWidget->setWidget(m_controller);

	m_window->addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

	connect(m_controller, &Controller::snapshotClicked, m_window, &PreProcessorGridCrosssectionWindow2::saveSnapshots);
	connect(m_controller, &Controller::csvExportClicked, m_window, &PreProcessorGridCrosssectionWindow2::exportCsvs);
}

void PreProcessorGridCrosssectionWindow2::Impl::setupDisplaySettingTableController()
{
	m_displaySettingTableController = new DisplaySettingTableController(m_window->ui->settingTable, this);
	m_displaySettingTableController->applyToTable();
}

void PreProcessorGridCrosssectionWindow2::Impl::setupEditTableController()
{
	m_editTableController = new EditTableController(m_window->ui->editTable, this);
}

void PreProcessorGridCrosssectionWindow2::Impl::setupDisplaySettings()
{
	auto gt = m_gridDataItem->gridTypeDataItem()->gridType();
	// node
	for (auto att : gt->gridAttributes()) {
		if (att->position() != SolverDefinitionGridAttribute::Position::Node) {continue;}

		GridAttributeDisplaySettingContainer setting(att);
		QString name = att->name().c_str();
		
		if (name.toLower().contains("elevation")) {
			setting.visible = true;
			setting.mode = GridAttributeDisplaySettingContainer::Mode::AsElevation;
		}
		m_displaySettings.push_back(setting);
	}
	// cell
	for (auto att : gt->gridAttributes()) {
		if (att->position() != SolverDefinitionGridAttribute::Position::CellCenter) {continue;}

		GridAttributeDisplaySettingContainer setting(att);
		setting.visible = false;
		m_displaySettings.push_back(setting);
	}
	for (auto& s : m_displaySettings) {
		m_displaySettingMap.insert({s.attribute()->name(), &s});
	}
}

PreProcessorGridCrosssectionWindow2::GraphicsView* PreProcessorGridCrosssectionWindow2::Impl::graphicsView() const
{
	return m_window->ui->graphicsView;
}

Structured2DGrid* PreProcessorGridCrosssectionWindow2::Impl::grid() const
{
	return dynamic_cast<Structured2DGrid*> (m_gridDataItem->grid());
}

void PreProcessorGridCrosssectionWindow2::Impl::updateAspectRatioEdit()
{
	auto ratio = m_window->ui->graphicsView->aspectRatio();
	m_aspectRatioEdit->setValue(ratio);
}

ColorMapSettingContainerI* PreProcessorGridCrosssectionWindow2::Impl::preColorMapSetting(const std::string& name)
{
	return m_gridDataItem->gridTypeDataItem()->colorMapSetting(name);
}
