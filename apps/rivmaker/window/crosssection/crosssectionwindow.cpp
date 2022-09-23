#include "crosssectionwindow.h"
#include "crosssectionwindowdisplaysettingdialog.h"
#include "crosssectionwindowgraphicsview.h"

#include "../../data/crosssection/crosssection.h"
#include "../../data/crosssections/crosssections.h"
#include "../../data/project/project.h"

#include <QColor>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QSvgGenerator>
#include <QTextStream>
#include <QToolBar>
#include <QVector2D>

#include <map>

CrossSectionWindow::CrossSectionWindow(QWidget *parent) :
	ChartWindow(parent),
	m_currentCrossSection {nullptr}
{
	setWindowIcon(QIcon(":/images/iconRiverCrosssection.svg"));

	auto graphicsView = new CrossSectionWindowGraphicsView(this);
	setCentralWidget(graphicsView);

	setupToolBars();

	connect(graphicsView, SIGNAL(positionChangedForStatusBar(QPointF)), this, SIGNAL(positionChangedForStatusBar(QPointF)));
	connect(m_crossSectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSelectionChange(int)));
	connect(m_exportSVGButton, SIGNAL(clicked()), this, SLOT(exportSVG()));
	connect(m_exportCSVButton, SIGNAL(clicked()), this, SLOT(exportCSV()));
	connect(m_displaySettingButton, SIGNAL(clicked()), this, SLOT(editDisplaySetting()));
	connect(m_WSECheckBox, SIGNAL(toggled(bool)), graphicsView, SLOT(update()));
	connect(m_LBHWMCheckBox, SIGNAL(toggled(bool)), graphicsView, SLOT(update()));
	connect(m_RBHWMCheckBox, SIGNAL(toggled(bool)), graphicsView, SLOT(update()));
}

CrossSectionWindow::~CrossSectionWindow()
{}

Project* CrossSectionWindow::project() const
{
	return m_project;
}

void CrossSectionWindow::setProject(Project* project)
{
	m_project = project;
	updateView();
}

CrossSection* CrossSectionWindow::currentCrossSection() const
{
	return m_currentCrossSection;
}

bool CrossSectionWindow::showWSE() const
{
	return m_WSECheckBox->isChecked();
}

bool CrossSectionWindow::showLBHWM() const
{
	return m_LBHWMCheckBox->isChecked();
}

bool CrossSectionWindow::showRBHWM() const
{
	return m_RBHWMCheckBox->isChecked();
}

void CrossSectionWindow::exportSVG()
{
	QString fname = QFileDialog::getSaveFileName(this, tr("Save SVG file"), QString(), tr("SVG files (*.svg)"));
	if (fname.isEmpty()) {return;}

	auto gv = graphicsView();
	QSvgGenerator generator;
	generator.setFileName(fname);
	generator.setSize(gv->size());
	generator.setViewBox(gv->rect());
	generator.setTitle("Cross Section Window Image");

	QPainter painter(&generator);
	gv->draw(&painter);
}

void CrossSectionWindow::exportCSV()
{
	QString fname = QFileDialog::getSaveFileName(this, tr("Save CSV file"), QString(), tr("CSV files (*.csv)"));
	if (fname.isEmpty()) {return;}
	QFile f(fname);
	if (! f.open(QFile::WriteOnly)) {
		QMessageBox::critical(this, tr("Error"), tr("%1 could note be opened.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	QTextStream ts(&f);
	ts.setRealNumberPrecision(6);
	auto points = m_currentCrossSection->mappedPoints();
	for (const auto& p : points) {
		ts << p.x() << "," << p.y() << endl;
	}
	f.close();
}

void CrossSectionWindow::editDisplaySetting()
{
	auto gv = graphicsView();
	CrossSectionWindowDisplaySettingDialog dialog(this);
	dialog.setSettings(gv->crossSectionWindowDisplaySetting(), gv->chartDisplaySetting());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	gv->setCrossSectionWindowDisplaySetting(dialog.crossSectionWindowDisplaySetting());
	gv->setChartDisplaySetting(dialog.chartGraphicsViewDisplaySetting());

	gv->update();
}

void CrossSectionWindow::updateView()
{
	// update combobox
	auto csVec = m_project->crossSections().crossSectionVector();
	if (csVec.size() == 0) {
		parentWidget()->close();
		return;
	}

	m_crossSectionComboBox->blockSignals(true);
	int idx = -1;
	m_crossSectionComboBox->clear();
	for (int i = 0; i < csVec.size(); ++i) {
		CrossSection* cs = csVec.at(i);
		m_crossSectionComboBox->addItem(cs->name());
		if (m_currentCrossSection == cs) {
			idx = i;
		}
	}
	if (idx == -1) {
		idx = 0;
		m_currentCrossSection = csVec.at(0);
	}
	m_crossSectionComboBox->setCurrentIndex(idx);
	m_crossSectionComboBox->blockSignals(false);

	graphicsView()->update();
	updateWindowTitle();
}

void CrossSectionWindow::handleSelectionChange(int selected)
{
	auto csVec = m_project->crossSections().crossSectionVector();
	m_currentCrossSection = csVec.at(selected);
	updateWindowTitle();

	fit();
}

void CrossSectionWindow::setupToolBars()
{
	QToolBar* crossSectionToolBar = new QToolBar(this);
	auto l = new QLabel(tr("Cross Section: "), crossSectionToolBar);
	crossSectionToolBar->addWidget(l);

	m_crossSectionComboBox = new QComboBox(this);
	m_crossSectionComboBox->setMinimumWidth(100);
	crossSectionToolBar->addWidget(m_crossSectionComboBox);

	crossSectionToolBar->addSeparator();

	m_exportSVGButton = new QPushButton(tr("Export &SVG..."), this);
	crossSectionToolBar->addWidget(m_exportSVGButton);

	m_exportCSVButton = new QPushButton(tr("Export &CSV..."), this);
	crossSectionToolBar->addWidget(m_exportCSVButton);

	addToolBar(Qt::TopToolBarArea, crossSectionToolBar);
	addToolBarBreak(Qt::TopToolBarArea);

	addToolBars();

	auto tb = displayToolBar();

	m_WSECheckBox = new QCheckBox(tr("WSE"), this);
	m_WSECheckBox->setChecked(true);
	tb->addWidget(m_WSECheckBox);

	m_LBHWMCheckBox = new QCheckBox(tr("Left Bank HWM"), this);
	m_LBHWMCheckBox->setChecked(true);
	tb->addWidget(m_LBHWMCheckBox);

	m_RBHWMCheckBox = new QCheckBox(tr("Right Bank HWM"), this);
	m_RBHWMCheckBox->setChecked(true);
	tb->addWidget(m_RBHWMCheckBox);

	m_displaySettingButton = new QPushButton("&Display Setting", this);
	tb->addWidget(m_displaySettingButton);

	m_fixAspectRatioCheckBox->setChecked(true);
	setAspectRatio(5.0);
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

CrossSectionWindowGraphicsView* CrossSectionWindow::graphicsView() const
{
	return dynamic_cast<CrossSectionWindowGraphicsView*>(centralWidget());
}
