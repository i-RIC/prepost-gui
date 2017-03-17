#include "verticalcrosssectionwindow.h"
#include "ui_verticalcrosssectionwindow.h"
#include "../../data/arbitraryhwm/arbitraryhwm.h"
#include "../../data/baseline/baseline.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/leftbankhwm/leftbankhwm.h"
#include "../../data/project/project.h"
#include "../../data/points/points.h"
#include "../../data/rightbankhwm/rightbankhwm.h"
#include "../../data/watersurfaceelevationpoints/watersurfaceelevationpoints.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include <QCloseEvent>
#include <QPointF>
#include <QVector>
#include <QVector3D>

namespace {
	void setSamples(const BaseLine& baseLine, const Points& points, QwtPlotCurve* curve, double* xmin, double* xmax, double* ymin, double* ymax, bool* first)
	{
		QVector<QPointF> samples;
		for (QVector3D* p : points.points()){
			double pos = baseLine.calcPosition(p->x(), p->y());
			samples.push_back(QPointF(pos, p->z()));

			if (*first || pos < *xmin) {*xmin = pos;}
			if (*first || pos > *xmax) {*xmax = pos;}
			if (*first || p->z() < *ymin) {*ymin = p->z();}
			if (*first || p->z() > *ymax) {*ymax = p->z();}

			*first = false;
		}
		curve->setSamples(samples);
	}
}

VerticalCrossSectionWindow::VerticalCrossSectionWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VerticalCrossSectionWindow)
{
	ui->setupUi(this);

	QList<int> sizes;
	sizes << 300 << 100;
	ui->splitter->setSizes(sizes);

	initPlot();
	initTable();
}

VerticalCrossSectionWindow::~VerticalCrossSectionWindow()
{
	delete m_arbitraryCurve;
	delete m_leftBankCurve;
	delete m_rightBankCurve;

	delete ui;
}

void VerticalCrossSectionWindow::setProject(Project* project)
{
	m_project = project;
	connect(m_project, SIGNAL(updated()), this, SLOT(updateView()));
	updateView();
}

void VerticalCrossSectionWindow::updateView()
{
	const auto& baseLine = m_project->baseLine();
	const auto& wse = m_project->waterSurfaceElevationPoints();

	double xmin = 0, xmax = 1, ymin = 0, ymax = 1;
	bool first = true;

	setSamples(baseLine, wse.arbitraryHWM(), m_arbitraryCurve, &xmin, &xmax, &ymin, &ymax, &first);
	setSamples(baseLine, wse.leftBankHWM(), m_leftBankCurve, &xmin, &xmax, &ymin, &ymax, &first);
	setSamples(baseLine, wse.rightBankHWM(), m_rightBankCurve, &xmin, &xmax, &ymin, &ymax, &first);

	setupCrossSectionMarkers(&xmin, &xmax, &first);

	updateScale(xmin, xmax, ymin, ymax);

	ui->qwtWidget->replot();
}

void VerticalCrossSectionWindow::initPlot()
{
	auto qwtW = ui->qwtWidget;
	qwtW->setAxisTitle(QwtPlot::xBottom, tr("Distance Upstream"));
	qwtW->enableAxis(QwtPlot::xBottom);

	qwtW->setAxisTitle(QwtPlot::yLeft, tr("Elevation"));
	qwtW->enableAxis(QwtPlot::yLeft);

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid->attach(ui->qwtWidget);

	QwtSymbol* s = nullptr;

	m_arbitraryCurve = new QwtPlotCurve();
	m_arbitraryCurve->setPen(Qt::transparent, 1);
	s = new QwtSymbol(QwtSymbol::Rect, QBrush(Qt::gray), QPen(Qt::NoPen), QSize(7, 7));
	m_arbitraryCurve->setSymbol(s);
	m_arbitraryCurve->attach(ui->qwtWidget);

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
}

void VerticalCrossSectionWindow::initTable()
{
	ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setRowCount(3);

	QStringList hLabels;
	hLabels << tr("Name") << tr("Elevation");
	ui->tableWidget->setHorizontalHeaderLabels(hLabels);

	ui->tableWidget->setItem(0, 0, new QTableWidgetItem("X1"));
	ui->tableWidget->setItem(1, 0, new QTableWidgetItem("X2"));
	ui->tableWidget->setItem(2, 0, new QTableWidgetItem("X3"));
}

void VerticalCrossSectionWindow::setupCrossSectionMarkers(double* xmin, double* xmax, bool* first)
{
	for (QwtPlotMarker* m : m_crossSectionMarkers) {
		m->detach();
		delete m;
	}
	const auto& baseLine = m_project->baseLine();
	auto csVec = m_project->crossSections().crossSectionVector();

	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		baseLine.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {continue;}

		QwtPlotMarker* marker = new QwtPlotMarker(cs->name());
		marker->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
		marker->setXValue(pos);
		marker->setLabelOrientation(Qt::Vertical);
		marker->setLineStyle(QwtPlotMarker::VLine);
		marker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
		marker->attach(ui->qwtWidget);

		if (*first || pos < *xmin) {*xmin = pos;}
		if (*first || pos > *xmax) {*xmax = pos;}

		*first = false;
	}
}

void VerticalCrossSectionWindow::updateScale(double xmin, double xmax, double ymin, double ymax)
{
	double xwidth = xmax - xmin;
	double ywidth = ymax - ymin;

	ui->qwtWidget->setAxisScale(QwtPlot::xBottom, xmin - xwidth * 0.1, xmax + xwidth * 0.1);
	ui->qwtWidget->setAxisScale(QwtPlot::yLeft, ymin - ywidth * 0.1, ymax + ywidth * 0.1);
}

void VerticalCrossSectionWindow::closeEvent(QCloseEvent *e)
{
	parentWidget()->hide();
	e->ignore();
}
