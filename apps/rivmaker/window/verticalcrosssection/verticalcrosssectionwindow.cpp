#include "verticalcrosssectionwindow.h"
#include "ui_verticalcrosssectionwindow.h"

VerticalCrossSectionWindow::VerticalCrossSectionWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VerticalCrossSectionWindow)
{
	ui->setupUi(this);
}

VerticalCrossSectionWindow::~VerticalCrossSectionWindow()
{
	delete ui;
}
