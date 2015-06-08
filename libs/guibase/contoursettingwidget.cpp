#include "ui_contoursettingwidget.h"

#include "contoursettingwidget.h"

ContourSettingWidget::ContourSettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ContourSettingWidget)
{
	ui->setupUi(this);
	connect(ui->pointsButton, SIGNAL(toggled(bool)), this, SLOT(handleContourChange()));
	connect(ui->isolinesButton, SIGNAL(toggled(bool)), this, SLOT(handleContourChange()));
	connect(ui->contourFigureButton, SIGNAL(toggled(bool)), this, SLOT(handleContourChange()));
	connect(ui->colorFringeButton, SIGNAL(toggled(bool)), this, SLOT(handleContourChange()));
}

ContourSettingWidget::~ContourSettingWidget()
{
	delete ui;
}

void ContourSettingWidget::setContour(Contour contour)
{
	switch (contour) {
	case Points:
		ui->pointsButton->setChecked(true);
		break;
	case Isolines:
		ui->isolinesButton->setChecked(true);
		break;
	case ContourFigure:
		ui->contourFigureButton->setChecked(true);
		break;
	case ColorFringe:
		ui->colorFringeButton->setChecked(true);
		break;
	}
}

void ContourSettingWidget::handleContourChange()
{
	if (ui->pointsButton->isChecked()) {
		emit contourChanged(Points);
	}
	if (ui->isolinesButton->isChecked()) {
		emit contourChanged(Isolines);
	}
	if (ui->contourFigureButton->isChecked()) {
		emit contourChanged(ContourFigure);
	}
	if (ui->colorFringeButton->isChecked()) {
		emit contourChanged(ColorFringe);
	}
	emit contourChanged();
}

ContourSettingWidget::Contour ContourSettingWidget::contour()
{
	if (ui->pointsButton->isChecked()) { return Points; }
	if (ui->isolinesButton->isChecked()) { return Isolines; }
	if (ui->contourFigureButton->isChecked()) { return ContourFigure; }
	if (ui->colorFringeButton->isChecked()) { return ColorFringe; }
	//default
	return Isolines;
}

void ContourSettingWidget::hidePoints()
{
	ui->pointsButton->hide();
}

void ContourSettingWidget::setNoPolyData(bool nopolydata)
{
	if (! nopolydata) {return;}
	ui->pointsButton->setChecked(true);
	ui->isolinesButton->setDisabled(true);
	ui->contourFigureButton->setDisabled(true);
	ui->colorFringeButton->setDisabled(true);
}
