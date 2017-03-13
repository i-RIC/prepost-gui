#include "verticalcrosssectionwindow.h"
#include "ui_verticalcrosssectionwindow.h"

#include <QCloseEvent>

VerticalCrossSectionWindow::VerticalCrossSectionWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VerticalCrossSectionWindow)
{
	ui->setupUi(this);

	QList<int> sizes;
	sizes << 300 << 100;
	ui->splitter->setSizes(sizes);
}

VerticalCrossSectionWindow::~VerticalCrossSectionWindow()
{
	delete ui;
}

void VerticalCrossSectionWindow::closeEvent(QCloseEvent *e)
{
	parentWidget()->hide();
	e->ignore();
}
