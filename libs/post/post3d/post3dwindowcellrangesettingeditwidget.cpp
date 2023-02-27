#include "post3dwindowcellrangesettingcontainer.h"
#include "post3dwindowcellrangesettingeditwidget.h"
#include "ui_post3dwindowcellrangesettingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkStructuredGrid.h>

Post3dWindowCellRangeSettingEditWidget::Post3dWindowCellRangeSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowCellRangeSettingWidget)
{
	ui->setupUi(this);

	connect(ui->iMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowCellRangeSettingEditWidget::iMinChanged);
	connect(ui->iMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowCellRangeSettingEditWidget::iMaxChanged);
	connect(ui->jMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowCellRangeSettingEditWidget::jMinChanged);
	connect(ui->jMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowCellRangeSettingEditWidget::jMaxChanged);
	connect(ui->kMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowCellRangeSettingEditWidget::kMinChanged);
	connect(ui->kMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowCellRangeSettingEditWidget::kMaxChanged);
}

Post3dWindowCellRangeSettingEditWidget::~Post3dWindowCellRangeSettingEditWidget()
{
	delete ui;
}

void Post3dWindowCellRangeSettingEditWidget::setZoneData(PostZoneDataContainer* zd)
{
	auto grid = dynamic_cast<vtkStructuredGrid*>(zd->data());

	setDimensions(grid->GetDimensions());
}

void Post3dWindowCellRangeSettingEditWidget::setDimensions(int* dims)
{
	int dim0 = *dims;
	int dim1 = *(dims + 1);
	int dim2 = *(dims + 2);

	ui->iMinSlider->setRange(1, dim0 - 1);
	ui->iMaxSlider->setRange(1, dim0 - 1);
	ui->jMinSlider->setRange(1, dim1 - 1);
	ui->jMaxSlider->setRange(1, dim1 - 1);
	ui->kMinSlider->setRange(1, dim2 - 1);
	ui->kMaxSlider->setRange(1, dim2 - 1);
}

Post3dWindowCellRangeSettingContainer Post3dWindowCellRangeSettingEditWidget::setting() const
{
	Post3dWindowCellRangeSettingContainer setting;
	setting.enabled = ui->enabledCheckBox->isChecked();
	setting.iMin = ui->iMinSlider->value() - 1;
	setting.iMax = ui->iMaxSlider->value() - 1;
	setting.jMin = ui->jMinSlider->value() - 1;
	setting.jMax = ui->jMaxSlider->value() - 1;
	setting.kMin = ui->kMinSlider->value() - 1;
	setting.kMax = ui->kMaxSlider->value() - 1;

	return setting;
}

void Post3dWindowCellRangeSettingEditWidget::setSetting(const Post3dWindowCellRangeSettingContainer& setting)
{
	ui->enabledCheckBox->setChecked(setting.enabled);
	ui->iMinSlider->setValue(setting.iMin + 1);
	ui->iMaxSlider->setValue(setting.iMax + 1);
	ui->jMinSlider->setValue(setting.jMin + 1);
	ui->jMaxSlider->setValue(setting.jMax + 1);
	ui->kMinSlider->setValue(setting.kMin + 1);
	ui->kMaxSlider->setValue(setting.kMax + 1);
}

void Post3dWindowCellRangeSettingEditWidget::iMinChanged(int imin)
{
	if (ui->iMaxSlider->value() < imin) {
		ui->iMaxSlider->setValue(imin);
	}
}

void Post3dWindowCellRangeSettingEditWidget::iMaxChanged(int imax)
{
	if (ui->iMinSlider->value() > imax) {
		ui->iMinSlider->setValue(imax);
	}
}

void Post3dWindowCellRangeSettingEditWidget::jMinChanged(int jmin)
{
	if (ui->jMaxSlider->value() < jmin) {
		ui->jMaxSlider->setValue(jmin);
	}
}

void Post3dWindowCellRangeSettingEditWidget::jMaxChanged(int jmax)
{
	if (ui->jMinSlider->value() > jmax) {
		ui->jMinSlider->setValue(jmax);
	}
}

void Post3dWindowCellRangeSettingEditWidget::kMinChanged(int kmin)
{
	if (ui->kMaxSlider->value() < kmin) {
		ui->kMaxSlider->setValue(kmin);
	}
}

void Post3dWindowCellRangeSettingEditWidget::kMaxChanged(int kmax)
{
	if (ui->kMinSlider->value() > kmax) {
		ui->kMinSlider->setValue(kmax);
	}
}
