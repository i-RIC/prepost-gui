#include "verticalcrosssectionwindow.h"
#include "verticalcrosssectionwindowdisplaysettingdockwidget.h"
#include "verticalcrosssectionwindowdisplaysettingdockwidgetview.h"
#include "verticalcrosssectionwindowdisplaysettingdialog.h"
#include "verticalcrosssectionwindowelevationsdockwidget.h"
#include "verticalcrosssectionwindowelevationsdockwidgetview.h"
#include "verticalcrosssectionwindowgraphicsview.h"
#include "../../data/arbitraryhwm/arbitraryhwm.h"
#include "../../data/benchmark/benchmark.h"
#include "../../data/baseline/baseline.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/crosssections/crosssections.h"
#include "../../data/hub/hub.h"
#include "../../data/leftbankhwm/leftbankhwm.h"
#include "../../data/project/project.h"
#include "../../data/points/points.h"
#include "../../data/referencemark/referencemark.h"
#include "../../data/rightbankhwm/rightbankhwm.h"
#include "../../data/streamgage/streamgage.h"
#include "../../data/watersurfaceelevationpoints/watersurfaceelevationpoints.h"
#include "../../geom/geometrypoint.h"
#include "../../main/rivmakermainwindow.h"
#include "../../misc/qwtcanvaswithpositionsignal.h"

#include <misc/informationdialog.h>

#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSvgGenerator>
#include <QTextStream>
#include <QToolBar>

namespace {

void exportCsv(bool show, const BaseLine& baseLine, const Points& points, const QString& label, QTextStream* ts)
{
	if (! show) {return;}

	bool l_internal;
	for (auto p : points.points()) {
		double pos = baseLine.calcPosition(p->x(), p->y(), &l_internal);
		*ts << pos << "," << p->z() << "," << p->name() << "," << label << endl;
	}
}

/*
	const int defaultRowHeight = 20;

	void clearMarkers(std::vector<QwtPlotMarker*>* markers)
	{
		for (QwtPlotMarker* marker : *markers) {
			marker->detach();
			delete marker;
		}
		markers->clear();
	}

	void setSamples(const BaseLine& baseLine, const Points& points, QwtPlotCurve* curve, std::vector<QwtPlotMarker*>* markers, double* xmin, double* xmax, double* ymin, double* ymax, QwtPlot* plot, bool* first)
	{
		clearMarkers(markers);

		QVector<QPointF> samples;
		bool l_internal;
		for (GeometryPoint* p : points.points()){
			double pos = baseLine.calcPosition(p->x(), p->y(), &l_internal);
			samples.push_back(QPointF(pos, p->z()));

			if (! p->name().isNull()) {
				QwtPlotMarker* marker = new QwtPlotMarker();
				marker->setLabel(p->name());
				marker->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
				marker->setXValue(pos);
				marker->setYValue(p->z());
				marker->setLabelOrientation(Qt::Horizontal);
				marker->setLineStyle(QwtPlotMarker::NoLine);
				marker->setSpacing(5);
				marker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
				marker->attach(plot);

				markers->push_back(marker);
			}

			if (*first || pos < *xmin) {*xmin = pos;}
			if (*first || pos > *xmax) {*xmax = pos;}
			if (*first || p->z() < *ymin) {*ymin = p->z();}
			if (*first || p->z() > *ymax) {*ymax = p->z();}

			*first = false;
		}
		curve->setSamples(samples);
	}

	class Delegate : public QStyledItemDelegate
	{
	public:
		Delegate(QObject* = nullptr) {}
		QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
		{
			if (index.column() == 0) {
				return nullptr;
				// return new QLabel(parent);
			} else {
				auto spinBox = new QDoubleSpinBox(parent);
				spinBox->setDecimals(3);
				spinBox->setSingleStep(0.01);
				spinBox->setMinimum(-1000);
				spinBox->setMaximum(10000);
				return spinBox;
			}
		}
		void setEditorData(QWidget* editor, const QModelIndex& index) const
		{
			QVariant dat = index.model()->data(index, Qt::DisplayRole);
			if (index.column() == 0) {
				auto l = dynamic_cast<QLabel*> (editor);
				l->setText(dat.toString());
			} else {
				auto sb = dynamic_cast<QDoubleSpinBox*> (editor);
				sb->setValue(dat.toDouble());
			}
		}
		void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
		{
			if (index.column() == 0) {return;}

			auto sb = dynamic_cast<QDoubleSpinBox*> (editor);
			model->setData(index, sb->value(), Qt::EditRole);
		}

		void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
		{
			editor->setGeometry(option.rect);
		}
	};
*/
}

VerticalCrossSectionWindow::VerticalCrossSectionWindow(RivmakerMainWindow *parent) :
	ChartWindow {parent},
	m_mainWindow {parent}
{
	setWindowIcon(QIcon(":/images/iconVerticalCrosssection.png"));
	setWindowTitle(tr("Elevation View Window"));

	auto graphicsView = new VerticalCrossSectionWindowGraphicsView(this);
	setCentralWidget(graphicsView);

	setupToolBars();
	setupDockWidgets();

	connect(graphicsView, SIGNAL(positionChangedForStatusBar(QPointF)), this, SIGNAL(positionChangedForStatusBar(QPointF)));
	connect(m_exportSVGButton, SIGNAL(clicked()), this, SLOT(exportSVG()));
	connect(m_exportCSVButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
	connect(m_displaySettingButton, SIGNAL(clicked()), this, SLOT(editDisplaySetting()));
}

VerticalCrossSectionWindow::~VerticalCrossSectionWindow()
{}

Project* VerticalCrossSectionWindow::project() const
{
	return m_project;
}

void VerticalCrossSectionWindow::setProject(Project* project)
{
	m_project = project;
	if (m_project == nullptr) {return;}

	setCsvExportEnabled(project->isCsvFileNameSet());

	connect(m_project, SIGNAL(updated()), this, SLOT(updateView()));
	connect(m_project, SIGNAL(csvFileNameSet(bool)), this, SLOT(setCsvExportEnabled(bool)));
	updateView();
}

bool VerticalCrossSectionWindow::showArbitrary() const
{
	return m_displaySettingDockWidget->view()->showArbitrary();
}

bool VerticalCrossSectionWindow::showBenchmark() const
{
	return m_displaySettingDockWidget->view()->showBenchmark();
}

bool VerticalCrossSectionWindow::showReferenceMark() const
{
	return m_displaySettingDockWidget->view()->showReferenceMark();
}

bool VerticalCrossSectionWindow::showHub() const
{
	return m_displaySettingDockWidget->view()->showHub();
}

bool VerticalCrossSectionWindow::showLeftHWMs() const
{
	return m_displaySettingDockWidget->view()->showLeftHWMs();
}

bool VerticalCrossSectionWindow::showRightHWMs() const
{
	return m_displaySettingDockWidget->view()->showRightHWMs();
}

bool VerticalCrossSectionWindow::showStreamGage() const
{
	return m_displaySettingDockWidget->view()->showStreamGage();
}

bool VerticalCrossSectionWindow::showXSLine() const
{
	return m_displaySettingDockWidget->view()->showXSLine();
}

bool VerticalCrossSectionWindow::showWSELine() const
{
	return m_displaySettingDockWidget->view()->showWSELine();
}

bool VerticalCrossSectionWindow::showLeftHWMLine() const
{
	return m_displaySettingDockWidget->view()->showLeftHWMLine();
}

bool VerticalCrossSectionWindow::showRightHWMLine() const
{
	return m_displaySettingDockWidget->view()->showRightHWMLine();
}

void VerticalCrossSectionWindow::exportSVG()
{
	QString fname = QFileDialog::getSaveFileName(this, tr("Save SVG file"), QString(), tr("SVG files (*.svg)"));
	if (fname.isEmpty()) {return;}

	auto gv = graphicsView();
	QSvgGenerator generator;
	generator.setFileName(fname);
	generator.setSize(gv->size());
	generator.setViewBox(gv->rect());
	generator.setTitle("Elevation View Window Image");

	QPainter painter(&generator);
	gv->draw(&painter);
}

void VerticalCrossSectionWindow::exportCSV()
{
	InformationDialog::information(this, tr("Information"), tr("Data for checked Check Boxes are exported."), "rivmaker_vxs_csvexport");

	QString fname = QFileDialog::getSaveFileName(this, tr("Save CSV file"), QString(), tr("CSV files (*.csv)"));
	if (fname.isEmpty()) {return;}
	QFile f(fname);
	if (! f.open(QFile::WriteOnly)) {
		QMessageBox::critical(this, tr("Error"), tr("%1 could note be opened.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	QTextStream ts(&f);
	ts << "position" << "," << "elevation" << "," << "name" << "," << "type" << endl;
	ts.setRealNumberPrecision(6);

	auto p = project();
	const auto& points = p->waterSurfaceElevationPoints();

	exportCsv(showArbitrary(), p->baseLine(), points.arbitraryHWM(), "Arbitrary HWM", &ts);
	exportCsv(showBenchmark(), p->baseLine(), points.benchmark(), "Benchmark", &ts);
	exportCsv(showReferenceMark(), p->baseLine(), points.referenceMark(), "Reference mark", &ts);
	exportCsv(showHub(), p->baseLine(), points.hub(), "Surveying or turning point", &ts);
	exportCsv(showLeftHWMs(), p->baseLine(), points.leftBankHWM(), "Left Bank HWM", &ts);
	exportCsv(showRightHWMs(), p->baseLine(), points.rightBankHWM(), "Right Bank HWM", &ts);
	exportCsv(showStreamGage(), p->baseLine(), points.streamGage(), "Stream gage", &ts);

	f.close();
}

void VerticalCrossSectionWindow::editDisplaySetting()
{
	auto gv = graphicsView();
	VerticalCrossSectionWindowDisplaySettingDialog dialog(this);
	dialog.setSettings(gv->chartDisplaySetting());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	gv->setChartDisplaySetting(dialog.chartGraphicsViewDisplaySetting());

	gv->update();
}

void VerticalCrossSectionWindow::updateView()
{
	updateGraphicsView();
	m_elevationsDockWidget->view()->updateTable();
}

void VerticalCrossSectionWindow::setCsvExportEnabled(bool enabled)
{
	m_elevationsDockWidget->view()->setCsvExportEnabled(enabled);
}

void VerticalCrossSectionWindow::exportWaterSurfaceElevation()
{
	m_mainWindow->exportWaterSurfaceElevationData();
}

void VerticalCrossSectionWindow::updateGraphicsView()
{
	graphicsView()->updateView();
}

/*
void VerticalCrossSectionWindow::handleTableEdit(QStandardItem* editedItem)
{
	if (editedItem->column() == 0) {return;}
	int row = editedItem->row();
	double val = editedItem->data(Qt::EditRole).toDouble();

	const auto& crossSections = m_project->crossSections();
	crossSections.crossSectionVector().at(row)->setWaterElevation(val);

	m_project->emitUpdated();
}
*/
/*
void VerticalCrossSectionWindow::exportWaterSurfaceElevation()
{
	m_mainWindow->exportWaterSurfaceElevationData();
}
*/
/*
void VerticalCrossSectionWindow::initPlot()
{
	auto qwtW = ui->qwtWidget;
	qwtW->setAxisTitle(QwtPlot::xBottom, tr("Distance Upstream"));
	qwtW->enableAxis(QwtPlot::xBottom);

	qwtW->setAxisTitle(QwtPlot::yLeft, tr("Elevation"));
	qwtW->enableAxis(QwtPlot::yLeft);

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->setMajorPen(Qt::lightGray, 1.0, Qt::SolidLine);
	grid->setMinorPen(Qt::lightGray, 1.0, Qt::DashLine);
	grid->attach(ui->qwtWidget);

	QwtSymbol* s = nullptr;

	m_csCurve = new QwtPlotCurve();
	m_csCurve->setPen(Qt::black, 1);
	s = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::black), QPen(Qt::black), QSize(7, 7));
	m_csCurve->setSymbol(s);
	m_csCurve->attach(ui->qwtWidget);

	m_arbitraryCurve = new QwtPlotCurve();
	m_arbitraryCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::Rect, QBrush(Qt::gray), QPen(Qt::NoPen), QSize(7, 7));
	m_arbitraryCurve->setSymbol(s);
	m_arbitraryCurve->attach(ui->qwtWidget);

	m_benchmarkCurve = new QwtPlotCurve();
	m_benchmarkCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::DTriangle, QBrush(Qt::black), QPen(Qt::NoPen), QSize(7, 7));
	m_benchmarkCurve->setSymbol(s);
	m_benchmarkCurve->attach(ui->qwtWidget);

	m_referenceMarkCurve = new QwtPlotCurve();
	m_referenceMarkCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::UTriangle, QBrush(Qt::black), QPen(Qt::NoPen), QSize(7, 7));
	m_referenceMarkCurve->setSymbol(s);
	m_referenceMarkCurve->attach(ui->qwtWidget);

	m_hubCurve = new QwtPlotCurve();
	m_hubCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::gray), QPen(Qt::NoPen), QSize(7, 7));
	m_hubCurve->setSymbol(s);
	m_hubCurve->attach(ui->qwtWidget);

	m_leftBankCurve = new QwtPlotCurve();
	m_leftBankCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::Rect, QBrush(Qt::blue), QPen(Qt::NoPen), QSize(7, 7));
	m_leftBankCurve->setSymbol(s);
	m_leftBankCurve->attach(ui->qwtWidget);

	m_rightBankCurve = new QwtPlotCurve();
	m_rightBankCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::Rect, QBrush(Qt::red), QPen(Qt::NoPen), QSize(7, 7));
	m_rightBankCurve->setSymbol(s);
	m_rightBankCurve->attach(ui->qwtWidget);

	m_streamGageCurve = new QwtPlotCurve();
	m_streamGageCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::Rect, QBrush(Qt::gray), QPen(Qt::NoPen), QSize(7, 7));
	m_streamGageCurve->setSymbol(s);
	m_streamGageCurve->attach(ui->qwtWidget);

	m_zoomer = new QwtPlotZoomer(ui->qwtWidget->canvas());
	m_zoomer->setRubberBandPen(QPen(Qt::black));
	m_zoomer->setTrackerPen(QPen(Qt::darkBlue));
	m_zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
}
*/
/*
void VerticalCrossSectionWindow::initTable()
{
	ui->tableView->setModel(&m_tableModel);

	auto delegate = new Delegate(this);
	ui->tableView->setItemDelegate(delegate);
}

void VerticalCrossSectionWindow::updatePlot()
{
	const auto& baseLine = m_project->baseLine();
	const auto& wse = m_project->waterSurfaceElevationPoints();

	double xmin = 0, xmax = 1, ymin = 0, ymax = 1;
	bool first = true;
	QVector<QPointF> emptySamples;

	if (ui->arbitraryCheckBox->isChecked()) {
		setSamples(baseLine, wse.arbitraryHWM(), m_arbitraryCurve, &m_arbitraryMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_arbitraryCurve->setSamples(emptySamples);
		clearMarkers(&m_arbitraryMarkers);
	}
	if (ui->benchmarkCheckBox->isChecked()) {
		setSamples(baseLine, wse.benchmark(), m_benchmarkCurve, &m_benchmarkMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_benchmarkCurve->setSamples(emptySamples);
		clearMarkers(&m_benchmarkMarkers);
	}
	if (ui->referenceMarkCheckBox->isChecked()) {
		setSamples(baseLine, wse.referenceMark(), m_referenceMarkCurve, &m_referenceMarkMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_referenceMarkCurve->setSamples(emptySamples);
		clearMarkers(&m_referenceMarkMarkers);
	}
	if (ui->hubCheckBox->isChecked()) {
		setSamples(baseLine, wse.hub(), m_hubCurve, &m_hubMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_hubCurve->setSamples(emptySamples);
		clearMarkers(&m_hubMarkers);
	}
	if (ui->leftCheckBox->isChecked()) {
		setSamples(baseLine, wse.leftBankHWM(), m_leftBankCurve, &m_leftBankMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_leftBankCurve->setSamples(emptySamples);
		clearMarkers(&m_leftBankMarkers);
	}
	if (ui->rightCheckBox->isChecked()) {
		setSamples(baseLine, wse.rightBankHWM(), m_rightBankCurve, &m_rightBankMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_rightBankCurve->setSamples(emptySamples);
		clearMarkers(&m_rightBankMarkers);
	}
	if (ui->streamGageCheckBox->isChecked()) {
		setSamples(baseLine, wse.streamGage(), m_streamGageCurve, &m_streamGageMarkers, &xmin, &xmax, &ymin, &ymax, ui->qwtWidget, &first);
	} else {
		m_streamGageCurve->setSamples(emptySamples);
		clearMarkers(&m_streamGageMarkers);
	}

	setupCrossSectionLine();
	setupCrossSectionMarkers(&xmin, &xmax, &first);

	updateScale(xmin, xmax, ymin, ymax);
}
*/

/*
void VerticalCrossSectionWindow::updateTable()
{
	const auto& crossSections = m_project->crossSections();

	auto& m = m_tableModel;
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

	ui->tableView->setModel(&m);
	ui->tableView->setColumnWidth(0, 50);
	ui->tableView->setColumnWidth(1, 100);
	for (int i = 0; i < m.rowCount(); ++i) {
		ui->tableView->setRowHeight(i, defaultRowHeight);
	}
}
*/

/*
void VerticalCrossSectionWindow::setupCrossSectionLine()
{
	const auto& baseLine = m_project->baseLine();
	auto csVec = m_project->crossSections().crossSectionVector();

	QVector<QPointF> samples;

	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		baseLine.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {continue;}

		samples.push_back(QPointF(pos, cs->waterElevation()));
	}
	m_csCurve->setSamples(samples);
}
*/
/*
void VerticalCrossSectionWindow::setupCrossSectionMarkers(double* xmin, double* xmax, bool* first)
{
	for (QwtPlotMarker* m : m_crossSectionMarkers) {
		m->detach();
		delete m;
	}
	m_crossSectionMarkers.clear();

	const auto& baseLine = m_project->baseLine();
	auto csVec = m_project->crossSections().crossSectionVector();

	bool crosses;
	double x, y, pos;

	QPen pen;
	pen.setWidth(2);
	pen.setStyle(Qt::DashLine);

	for (CrossSection* cs : csVec) {
		baseLine.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {continue;}

		QwtPlotMarker* marker = new QwtPlotMarker();
		marker->setLabel(cs->name());
		marker->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
		marker->setXValue(pos);
		marker->setLabelOrientation(Qt::Vertical);
		marker->setLineStyle(QwtPlotMarker::VLine);
		marker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
		marker->setLinePen(pen);
		marker->attach(ui->qwtWidget);

		m_crossSectionMarkers.push_back(marker);

		if (*first || pos < *xmin) {*xmin = pos;}
		if (*first || pos > *xmax) {*xmax = pos;}

		*first = false;
	}
}
*/
/*
void VerticalCrossSectionWindow::updateScale(double xmin, double xmax, double ymin, double ymax)
{
	double xwidth = xmax - xmin;
	double ywidth = ymax - ymin;
	double marginRate = 0.08;

	xmin -= xwidth * marginRate;
	xmax += xwidth * marginRate;
	ymin -= ywidth * marginRate;
	ymax += ywidth * marginRate;

	ui->qwtWidget->setAxisScale(QwtPlot::xBottom, xmin, xmax);
	ui->qwtWidget->setAxisScale(QwtPlot::yLeft, ymin, ymax);

	ui->qwtWidget->replot();
	m_zoomer->setZoomBase(QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax)));
}
*/

void VerticalCrossSectionWindow::closeEvent(QCloseEvent *e)
{
	parentWidget()->hide();
	e->ignore();
}

VerticalCrossSectionWindowGraphicsView* VerticalCrossSectionWindow::graphicsView() const
{
	return dynamic_cast<VerticalCrossSectionWindowGraphicsView*> (centralWidget());
}

void VerticalCrossSectionWindow::setupToolBars()
{
	QToolBar* exportToolBar = new QToolBar(this);

	m_exportSVGButton = new QPushButton(tr("Export &SVG..."), this);
	exportToolBar->addWidget(m_exportSVGButton);

	m_exportCSVButton = new QPushButton(tr("Export &CSV..."), this);
	exportToolBar->addWidget(m_exportCSVButton);

	addToolBar(Qt::TopToolBarArea, exportToolBar);
	addToolBarBreak(Qt::TopToolBarArea);

	addToolBars();

	auto tb = displayToolBar();

	m_displaySettingButton = new QPushButton("&Display Setting", this);
	tb->addWidget(m_displaySettingButton);
}

void VerticalCrossSectionWindow::setupDockWidgets()
{
	m_displaySettingDockWidget = new VerticalCrossSectionWindowDisplaySettingDockWidget(this);
	addDockWidget(Qt::RightDockWidgetArea, m_displaySettingDockWidget);

	m_elevationsDockWidget = new VerticalCrossSectionWindowElevationsDockWidget(this);
	addDockWidget(Qt::RightDockWidgetArea, m_elevationsDockWidget);
}
