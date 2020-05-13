#include "verticalcrosssectionwindow.h"
#include "verticalcrosssectionwindowelevationsdockwidgetview.h"
#include "private/verticalcrosssectionwindowelevationsdockwidgetview_itemdelegete.h"
#include "../../data/project/project.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/crosssections/crosssections.h"
#include "ui_verticalcrosssectionwindowelevationsdockwidgetview.h"

namespace {
const int DEFAULT_ROW_HEIGHT = 20;

} // namespace

VerticalCrossSectionWindowElevationsDockWidgetView::VerticalCrossSectionWindowElevationsDockWidgetView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VerticalCrossSectionWindowElevationsDockWidgetView)
{
	ui->setupUi(this);
	initTable();
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportCsv()));
}

VerticalCrossSectionWindowElevationsDockWidgetView::~VerticalCrossSectionWindowElevationsDockWidgetView()
{
	delete ui;
}

void VerticalCrossSectionWindowElevationsDockWidgetView::updateTable()
{
	auto w = chartWindow();
	const auto& crossSections = w->project()->crossSections();

	auto& m = m_model;
	m.blockSignals(true);
	
	m.clear();

	ui->tableView->setModel(nullptr);

	m.setColumnCount(2);
	QStringList hLabels;
	hLabels << tr("Name") << tr("Elevation");
	m.setHorizontalHeaderLabels(hLabels);

	int row = 0;
	for (CrossSection* cs : crossSections.crossSectionVector()) {
		m.insertRow(row);
		auto nameItem = new QStandardItem(cs->name());
		nameItem->setEditable(false);
		m.setItem(row, 0, nameItem);

		auto elevItem = new QStandardItem();
		elevItem->setData(cs->waterElevation(), Qt::EditRole);
		m.setItem(row, 1, elevItem);

		++ row;
	}
	m.blockSignals(false);

	ui->tableView->setModel(&m_model);
	ui->tableView->setColumnWidth(0, 50);
	ui->tableView->setColumnWidth(1, 100);
	for (int i = 0; i < m.rowCount(); ++i) {
		ui->tableView->setRowHeight(1, DEFAULT_ROW_HEIGHT);
	}
}

void VerticalCrossSectionWindowElevationsDockWidgetView::setCsvExportEnabled(bool enabled)
{
	ui->exportButton->setEnabled(enabled);
}

void VerticalCrossSectionWindowElevationsDockWidgetView::handleTableEdit(QStandardItem* editedItem)
{
	if (editedItem->column() == 0) {return;}
	int row = editedItem->row();
	double val = editedItem->data(Qt::EditRole).toDouble();

	auto p = chartWindow()->project();
	const auto& crossSections = p->crossSections();
	crossSections.crossSectionVector().at(row)->setWaterElevation(val);
	p->emitUpdated();
}

void VerticalCrossSectionWindowElevationsDockWidgetView::exportCsv()
{
	chartWindow()->exportWaterSurfaceElevation();
}

void VerticalCrossSectionWindowElevationsDockWidgetView::initTable()
{
	ui->tableView->setModel(&m_model);

	auto delegate = new ItemDelegete(this);
	ui->tableView->setItemDelegate(delegate);

	connect(&m_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(handleTableEdit(QStandardItem*)));
}

VerticalCrossSectionWindow* VerticalCrossSectionWindowElevationsDockWidgetView::chartWindow() const
{
	return dynamic_cast<VerticalCrossSectionWindow*> (parent()->parent());
}
