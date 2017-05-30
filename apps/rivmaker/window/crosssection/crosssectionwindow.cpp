#include "crosssectionwindow.h"
#include "ui_crosssectionwindow.h"

#include "../../data/crosssection/crosssection.h"
#include "../../data/crosssections/crosssections.h"
#include "../../data/project/project.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

#include <QVector2D>

#include <map>

CrossSectionWindow::CrossSectionWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CrossSectionWindow),
	m_currentCrossSection {nullptr}
{
	ui->setupUi(this);
	connect(ui->crossSectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChange(int)));

	initCurve();
}

CrossSectionWindow::~CrossSectionWindow()
{
	delete ui;
}

void CrossSectionWindow::setProject(Project* project)
{
	m_project = project;
	updateView();
}

void CrossSectionWindow::updateView()
{
	// update combobox
	auto csVec = m_project->crossSections().crossSectionVector();
	if (csVec.size() == 0) {
		parentWidget()->close();
		return;
	}

	ui->crossSectionComboBox->blockSignals(true);

	int idx = -1;
	ui->crossSectionComboBox->clear();
	for (int i = 0; i < csVec.size(); ++i) {
		CrossSection* cs = csVec.at(i);
		ui->crossSectionComboBox->addItem(cs->name());
		if (m_currentCrossSection == cs) {
			idx = i;
		}
	}
	if (idx == -1) {
		idx = 0;
		m_currentCrossSection = csVec.at(0);
	}
	ui->crossSectionComboBox->setCurrentIndex(idx);
	ui->crossSectionComboBox->blockSignals(false);

	updateCurve();
	updateWindowTitle();
}

void CrossSectionWindow::handleSelectionChange(int selected)
{
	auto csVec = m_project->crossSections().crossSectionVector();
	m_currentCrossSection = csVec.at(selected);

	updateView();
}

void CrossSectionWindow::initCurve()
{
	auto qwtW = ui->qwtWidget;
	qwtW->setAxisTitle(QwtPlot::xBottom, tr("Distance from Left bank"));
	qwtW->enableAxis(QwtPlot::xBottom);

	qwtW->setAxisTitle(QwtPlot::yLeft, tr("Elevation"));
	qwtW->enableAxis(QwtPlot::yLeft);

	QwtPlotGrid* grid = new QwtPlotGrid();
	grid->enableXMin(true);
	grid->enableYMin(true);
	grid->setMajorPen(Qt::lightGray, 1.0, Qt::SolidLine);
	grid->setMinorPen(Qt::lightGray, 1.0, Qt::DashLine);
	grid->attach(ui->qwtWidget);

	m_curve = new QwtPlotCurve();
	m_curve->setPen(Qt::black);

	QwtSymbol* s = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::black), QPen(Qt::black), QSize(7, 7));
	m_curve->setSymbol(s);
	m_curve->attach(ui->qwtWidget);

	m_waterElevationMarker = new QwtPlotMarker();
	m_waterElevationMarker->setYValue(0);
	m_waterElevationMarker->setLineStyle(QwtPlotMarker::HLine);
	m_waterElevationMarker->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);
	m_waterElevationMarker->setLinePen(QPen(Qt::blue));
	m_waterElevationMarker->attach(ui->qwtWidget);
}

void CrossSectionWindow::updateCurve()
{
	if (m_currentCrossSection == nullptr) {return;}

	QVector<QPointF> samples;

	for (auto p : m_currentCrossSection->mappedPoints()){
		samples.push_back(QPointF(p.x(), p.y()));
	}
	m_curve->setSamples(samples);

	m_waterElevationMarker->setYValue(m_currentCrossSection->waterElevation());

	ui->qwtWidget->replot();
}

void CrossSectionWindow::updateWindowTitle()
{
	if (m_currentCrossSection == nullptr) {
		setWindowTitle(tr("Cross Section Window"));
		return;
	}
	auto title = tr("Cross Section Window : %1").arg(m_currentCrossSection->name());
	setWindowTitle(title);
}
