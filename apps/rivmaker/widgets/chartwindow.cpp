#include "chartwindow.h"
#include "chartgraphicsview.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>

ChartWindow::ChartWindow(QWidget* parent) :
	m_displayToolBar {nullptr},
	m_viewToolBar {nullptr}
{}

void ChartWindow::fit()
{
	graphicsView()->cameraFit();
}

void ChartWindow::resetRotation()
{}

void ChartWindow::rotate90()
{}

void ChartWindow::zoomIn()
{
	graphicsView()->cameraZoomIn();
}

void ChartWindow::zoomInX()
{
	graphicsView()->cameraZoomInX();
}

void ChartWindow::zoomInY()
{
	graphicsView()->cameraZoomInY();
}

void ChartWindow::zoomOut()
{
	graphicsView()->cameraZoomOut();
}

void ChartWindow::zoomOutX()
{
	graphicsView()->cameraZoomOutX();
}

void ChartWindow::zoomOutY()
{
	graphicsView()->cameraZoomOutY();
}

bool ChartWindow::showGrid() const
{
	return m_showGridCheckBox->isChecked();
}

bool ChartWindow::showScales() const
{
	return m_showScalesCheckBox->isChecked();
}

bool ChartWindow::showAspectRatio() const
{
	return m_showAspectRatioCheckBox->isChecked();
}

bool ChartWindow::fixAspectRatio() const
{
	return m_fixAspectRatioCheckBox->isChecked();
}

bool ChartWindow::fixRegion() const
{
	return m_fixRegionCheckBox->isChecked();
}

void ChartWindow::setAspectRatio(double ratio)
{
	m_aspectRatioEdit->setText(QString::number(ratio));
}

void ChartWindow::updateChart()
{
	centralWidget()->update();
}

void ChartWindow::handleAspectRatioChange(const QString& newValue)
{
	double v = newValue.toDouble();
	graphicsView()->setAspectRatio(v);
}

void ChartWindow::updateAspectRatioStatus()
{
	m_aspectRatioEdit->setDisabled(m_fixAspectRatioCheckBox->isChecked() || m_fixRegionCheckBox->isChecked());
}

void ChartWindow::addToolBars()
{
	addViewToolBar();
	addToolBarBreak(Qt::TopToolBarArea);
	addDisplayToolBar();
}

QToolBar* ChartWindow::displayToolBar() const
{
	return m_displayToolBar;
}

QToolBar* ChartWindow::viewToolBar() const
{
	return m_viewToolBar;
}

void ChartWindow::addDisplayToolBar()
{
	m_displayToolBar = new QToolBar(tr("Display ToolBar"), this);

	m_showGridCheckBox = new QCheckBox(this);
	m_showGridCheckBox->setText(tr("Grid"));
	m_showGridCheckBox->setChecked(true);
	m_displayToolBar->addWidget(m_showGridCheckBox);
	connect(m_showGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateChart()));

	m_showScalesCheckBox = new QCheckBox(this);
	m_showScalesCheckBox->setText(tr("Scale"));
	m_showScalesCheckBox->setChecked(true);
	m_displayToolBar->addWidget(m_showScalesCheckBox);
	connect(m_showScalesCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateChart()));

	m_showAspectRatioCheckBox = new QCheckBox(this);
	m_showAspectRatioCheckBox->setText(tr("Aspect ratio"));
	m_showAspectRatioCheckBox->setChecked(true);
	m_displayToolBar->addWidget(m_showAspectRatioCheckBox);
	connect(m_showAspectRatioCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateChart()));

	addToolBar(Qt::TopToolBarArea, m_displayToolBar);
}

void ChartWindow::addViewToolBar()
{
	m_viewToolBar = new QToolBar(tr("View ToolBar"), this);

	auto l = new QLabel(tr("Aspect ratio (V/H): 1 / "), this);
	m_viewToolBar->addWidget(l);

	m_aspectRatioEdit = new QLineEdit(this);
	m_aspectRatioEdit->setMaximumWidth(100);
	m_viewToolBar->addWidget(m_aspectRatioEdit);

	connect(m_aspectRatioEdit, SIGNAL(textChanged(QString)), this, SLOT(handleAspectRatioChange(QString)));

	auto spacer = new QWidget(this);
	spacer->setFixedWidth(10);
	m_viewToolBar->addWidget(spacer);

	m_fixAspectRatioCheckBox = new QCheckBox(tr("Fix aspect ratio"), this);
	m_viewToolBar->addWidget(m_fixAspectRatioCheckBox);

	m_fixRegionCheckBox = new QCheckBox(tr("Fix region"), this);
	m_viewToolBar->addWidget(m_fixRegionCheckBox);

	connect(m_fixAspectRatioCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateAspectRatioStatus()));
	connect(m_fixRegionCheckBox, SIGNAL(toggled(bool)), m_fixAspectRatioCheckBox, SLOT(setDisabled(bool)));
	connect(m_fixRegionCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateAspectRatioStatus()));

	addToolBar(m_viewToolBar);

	m_fixAspectRatioCheckBox->setChecked(true);
	setAspectRatio(5.0);
}

ChartGraphicsView* ChartWindow::graphicsView()
{
	return dynamic_cast<ChartGraphicsView*> (centralWidget());
}
