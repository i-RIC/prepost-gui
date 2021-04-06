#include "post3dwindowcellrangesettingwidget.h"
#include "ui_post3dwindowcellrangesettingwidget.h"
#include "../post3dcellrangesettingcontainer.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkStructuredGrid.h>

Post3dWindowCellRangeSettingWidget::Post3dWindowCellRangeSettingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowCellRangeSettingWidget)
{
	ui->setupUi(this);

	connect(ui->iMinSlider, SIGNAL(valueChanged(int)), this, SLOT(iMinChanged(int)));
	connect(ui->iMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(iMaxChanged(int)));
	connect(ui->jMinSlider, SIGNAL(valueChanged(int)), this, SLOT(jMinChanged(int)));
	connect(ui->jMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(jMaxChanged(int)));
	connect(ui->kMinSlider, SIGNAL(valueChanged(int)), this, SLOT(kMinChanged(int)));
	connect(ui->kMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(kMaxChanged(int)));
}

Post3dWindowCellRangeSettingWidget::~Post3dWindowCellRangeSettingWidget()
{
	delete ui;
}

void Post3dWindowCellRangeSettingWidget::setZoneData(PostZoneDataContainer* zd)
{
	auto grid = dynamic_cast<vtkStructuredGrid*>(zd->data());
	int dims[3];
	grid->GetDimensions(dims);

	ui->iMinSlider->setRange(1, dims[0]-1);
	ui->iMaxSlider->setRange(1, dims[0]-1);
	ui->jMinSlider->setRange(1, dims[1]-1);
	ui->jMaxSlider->setRange(1, dims[1]-1);
	ui->kMinSlider->setRange(1, dims[2]-1);
	ui->kMaxSlider->setRange(1, dims[2]-1);
}

void Post3dWindowCellRangeSettingWidget::setSetting(const Post3dCellRangeSettingContainer& setting)
{
	ui->enabledCheckBox->setChecked(setting.enabled);
	ui->iMinSlider->setValue(setting.iMin + 1);
	ui->iMaxSlider->setValue(setting.iMax + 1);
	ui->jMinSlider->setValue(setting.jMin + 1);
	ui->jMaxSlider->setValue(setting.jMax + 1);
	ui->kMinSlider->setValue(setting.kMin + 1);
	ui->kMaxSlider->setValue(setting.kMax + 1);
}
Post3dCellRangeSettingContainer Post3dWindowCellRangeSettingWidget::setting() const
{
	Post3dCellRangeSettingContainer setting;
	setting.enabled = ui->enabledCheckBox->isChecked();
	setting.iMin = ui->iMinSlider->value() - 1;
	setting.iMax = ui->iMaxSlider->value() - 1;
	setting.jMin = ui->jMinSlider->value() - 1;
	setting.jMax = ui->jMaxSlider->value() - 1;
	setting.kMin = ui->kMinSlider->value() - 1;
	setting.kMax = ui->kMaxSlider->value() - 1;

	return setting;
}

void Post3dWindowCellRangeSettingWidget::iMinChanged(int imin)
{
	if (ui->iMaxSlider->value() < imin) {
		ui->iMaxSlider->setValue(imin);
	}
}

void Post3dWindowCellRangeSettingWidget::iMaxChanged(int imax)
{
	if (ui->iMinSlider->value() > imax) {
		ui->iMinSlider->setValue(imax);
	}
}

void Post3dWindowCellRangeSettingWidget::jMinChanged(int jmin)
{
	if (ui->jMaxSlider->value() < jmin) {
		ui->jMaxSlider->setValue(jmin);
	}
}

void Post3dWindowCellRangeSettingWidget::jMaxChanged(int jmax)
{
	if (ui->jMinSlider->value() > jmax) {
		ui->jMinSlider->setValue(jmax);
	}
}

void Post3dWindowCellRangeSettingWidget::kMinChanged(int kmin)
{
	if (ui->kMaxSlider->value() < kmin) {
		ui->kMaxSlider->setValue(kmin);
	}
}

void Post3dWindowCellRangeSettingWidget::kMaxChanged(int kmax)
{
	if (ui->kMinSlider->value() > kmax) {
		ui->kMinSlider->setValue(kmax);
	}
}
