#include "ui_structuredgridregionselectwidget.h"

#include "structuredgridregionselectwidget.h"

StructuredGridRegionSelectWidget::StructuredGridRegionSelectWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::StructuredGridRegionSelectWidget)
{
	ui->setupUi(this);
	connect(ui->iMinSlider, SIGNAL(valueChanged(int)), this, SLOT(handleIMinChange(int)));
	connect(ui->iMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(handleIMaxChange(int)));
	connect(ui->jMinSlider, SIGNAL(valueChanged(int)), this, SLOT(handleJMinChange(int)));
	connect(ui->jMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(handleJMaxChange(int)));
}

StructuredGridRegionSelectWidget::~StructuredGridRegionSelectWidget()
{
	delete ui;
}

void StructuredGridRegionSelectWidget::setGridDimensions(int idim, int jdim)
{
	ui->iMinSlider->setRange(1, idim);
	ui->iMaxSlider->setRange(1, idim);
	ui->jMinSlider->setRange(1, jdim);
	ui->jMaxSlider->setRange(1, jdim);
}

void StructuredGridRegionSelectWidget::selectAll()
{
	ui->iMinSlider->setValue(ui->iMinSlider->minimum());
	ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
	ui->jMinSlider->setValue(ui->jMinSlider->minimum());
	ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
}

void StructuredGridRegionSelectWidget::setiMin(int imin)
{
	ui->iMinSlider->setValue(imin + 1);
}

void StructuredGridRegionSelectWidget::setiMax(int imax)
{
	ui->iMaxSlider->setValue(imax + 1);
}

void StructuredGridRegionSelectWidget::setjMin(int jmin)
{
	ui->jMinSlider->setValue(jmin + 1);
}

void StructuredGridRegionSelectWidget::setjMax(int jmax)
{
	ui->jMaxSlider->setValue(jmax + 1);
}

void StructuredGridRegionSelectWidget::setRegion(const StructuredGridRegion::Range2d& region)
{
	setiMin(region.iMin);
	setiMax(region.iMax);
	setjMin(region.jMin);
	setjMax(region.jMax);
}

int StructuredGridRegionSelectWidget::iMin()
{
	return ui->iMinSlider->value() - 1;
}

int StructuredGridRegionSelectWidget::iMax()
{
	return ui->iMaxSlider->value()- 1;
}

int StructuredGridRegionSelectWidget::jMin()
{
	return ui->jMinSlider->value() - 1;
}

int StructuredGridRegionSelectWidget::jMax()
{
	return ui->jMaxSlider->value() - 1;
}

StructuredGridRegion::Range2d StructuredGridRegionSelectWidget::region()
{
	StructuredGridRegion::Range2d r;
	r.iMin = iMin();
	r.iMax = iMax();
	r.jMin = jMin();
	r.jMax = jMax();
	return r;
}

void StructuredGridRegionSelectWidget::handleIMinChange(int imin)
{
	if (ui->iMaxSlider->value() < imin) {
		ui->iMaxSlider->setValue(imin);
	}
}

void StructuredGridRegionSelectWidget::handleIMaxChange(int imax)
{
	if (ui->iMinSlider->value() > imax) {
		ui->iMinSlider->setValue(imax);
	}
}

void StructuredGridRegionSelectWidget::handleJMinChange(int jmin)
{
	if (ui->jMaxSlider->value() < jmin) {
		ui->jMaxSlider->setValue(jmin);
	}
}

void StructuredGridRegionSelectWidget::handleJMaxChange(int jmax)
{
	if (ui->jMinSlider->value() > jmax) {
		ui->jMinSlider->setValue(jmax);
	}
}
