#include "verticalcrosssectionwindow.h"
#include "verticalcrosssectionwindowdisplaysettingdockwidgetview.h"
#include "ui_verticalcrosssectionwindowdisplaysettingdockwidgetview.h"


VerticalCrossSectionWindowDisplaySettingDockWidgetView::VerticalCrossSectionWindowDisplaySettingDockWidgetView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::VerticalCrossSectionWindowDisplaySettingDockWidgetView)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	ui->setupUi(this);

	connect(ui->arbitraryCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->benchmarkCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->referenceMarkCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->hubCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->leftCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->rightCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->streamGageCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->xsLineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->wseLineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->leftLineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
	connect(ui->rightLineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateGraphicsView()));
}

VerticalCrossSectionWindowDisplaySettingDockWidgetView::~VerticalCrossSectionWindowDisplaySettingDockWidgetView()
{
	delete ui;
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showArbitrary() const
{
	return ui->arbitraryCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showBenchmark() const
{
	return ui->benchmarkCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showReferenceMark() const
{
	return ui->referenceMarkCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showHub() const
{
	return ui->hubCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showLeftHWMs() const
{
	return ui->leftCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showRightHWMs() const
{
	return ui->rightCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showStreamGage() const
{
	return ui->streamGageCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showXSLine() const
{
	return ui->xsLineCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showWSELine() const
{
	return ui->wseLineCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showLeftHWMLine() const
{
	return ui->leftLineCheckBox->isChecked();
}

bool VerticalCrossSectionWindowDisplaySettingDockWidgetView::showRightHWMLine() const
{
	return ui->rightLineCheckBox->isChecked();
}

void VerticalCrossSectionWindowDisplaySettingDockWidgetView::updateGraphicsView()
{
	auto w = dynamic_cast<VerticalCrossSectionWindow*> (parent()->parent());
	w->updateGraphicsView();
}
