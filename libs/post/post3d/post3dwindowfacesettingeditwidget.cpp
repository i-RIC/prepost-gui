#include "post3dwindowfacesettingcontainer.h"
#include "post3dwindowfacesettingeditwidget.h"
#include "ui_post3dwindowfacesettingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post3dWindowFaceSettingEditWidget::Post3dWindowFaceSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowFaceSettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->iMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowFaceSettingEditWidget::iMinChanged);
	connect(ui->iMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowFaceSettingEditWidget::iMaxChanged);
	connect(ui->jMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowFaceSettingEditWidget::jMinChanged);
	connect(ui->jMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowFaceSettingEditWidget::jMaxChanged);
	connect(ui->kMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowFaceSettingEditWidget::kMinChanged);
	connect(ui->kMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowFaceSettingEditWidget::kMaxChanged);

	connect(ui->iRadioButton, &QRadioButton::clicked, this, &Post3dWindowFaceSettingEditWidget::iDirClicked);
	connect(ui->jRadioButton, &QRadioButton::clicked, this, &Post3dWindowFaceSettingEditWidget::jDirClicked);
	connect(ui->kRadioButton, &QRadioButton::clicked, this, &Post3dWindowFaceSettingEditWidget::kDirClicked);
}

Post3dWindowFaceSettingEditWidget::~Post3dWindowFaceSettingEditWidget()
{
	delete ui;
}

void Post3dWindowFaceSettingEditWidget::setZoneData(PostZoneDataContainer* zd)
{
	auto grid = dynamic_cast<vtkStructuredGrid*>(zd->data());

	setDimensions(grid->GetDimensions());
}

void Post3dWindowFaceSettingEditWidget::setDimensions(int* dims)
{
	int dim0 = *dims;
	int dim1 = *(dims + 1);
	int dim2 = *(dims + 2);

	ui->iMinSlider->setRange(1, dim0);
	ui->iMaxSlider->setRange(1, dim0);
	ui->jMinSlider->setRange(1, dim1);
	ui->jMaxSlider->setRange(1, dim1);
	ui->kMinSlider->setRange(1, dim2);
	ui->kMaxSlider->setRange(1, dim2);
}

Post3dWindowFaceSettingContainer Post3dWindowFaceSettingEditWidget::setting() const
{
	Post3dWindowFaceSettingContainer setting;

	setting.direction = direction();

	setting.enabled = true;
	setting.iMin = ui->iMinSlider->value() - 1;
	setting.iMax = ui->iMaxSlider->value() - 1;
	setting.jMin = ui->jMinSlider->value() - 1;
	setting.jMax = ui->jMaxSlider->value() - 1;
	setting.kMin = ui->kMinSlider->value() - 1;
	setting.kMax = ui->kMaxSlider->value() - 1;

	return setting;
}

void Post3dWindowFaceSettingEditWidget::setSetting(const Post3dWindowFaceSettingContainer& setting)
{
	if (setting.direction == Post3dWindowFaceSettingContainer::Direction::dirI) {
		ui->iRadioButton->setChecked(true);
	} else if (setting.direction == Post3dWindowFaceSettingContainer::Direction::dirJ) {
		ui->jRadioButton->setChecked(true);
	} else if (setting.direction == Post3dWindowFaceSettingContainer::Direction::dirK) {
		ui->kRadioButton->setChecked(true);
	}

	ui->iMinSlider->blockSignals(true);
	ui->iMaxSlider->blockSignals(true);
	ui->jMinSlider->blockSignals(true);
	ui->jMaxSlider->blockSignals(true);
	ui->kMinSlider->blockSignals(true);
	ui->kMaxSlider->blockSignals(true);

	ui->iMinSlider->setValue(setting.iMin + 1);
	ui->iMaxSlider->setValue(setting.iMax + 1);
	ui->jMinSlider->setValue(setting.jMin + 1);
	ui->jMaxSlider->setValue(setting.jMax + 1);
	ui->kMinSlider->setValue(setting.kMin + 1);
	ui->kMaxSlider->setValue(setting.kMax + 1);

	ui->iMinSlider->blockSignals(false);
	ui->iMaxSlider->blockSignals(false);
	ui->jMinSlider->blockSignals(false);
	ui->jMaxSlider->blockSignals(false);
	ui->kMinSlider->blockSignals(false);
	ui->kMaxSlider->blockSignals(false);

	applyDirectionSetting();
}

void Post3dWindowFaceSettingEditWidget::iMinChanged(int imin)
{
	if (direction() == Post3dWindowFaceSettingContainer::Direction::dirI || ui->iMaxSlider->value() < imin) {
		ui->iMaxSlider->blockSignals(true);
		ui->iMaxSlider->setValue(imin);
		ui->iMaxSlider->blockSignals(false);
	}
}

void Post3dWindowFaceSettingEditWidget::iMaxChanged(int imax)
{
	if (direction() == Post3dWindowFaceSettingContainer::Direction::dirI || ui->iMinSlider->value() > imax) {
		ui->iMinSlider->blockSignals(true);
		ui->iMinSlider->setValue(imax);
		ui->iMinSlider->blockSignals(false);
	}
}

void Post3dWindowFaceSettingEditWidget::jMinChanged(int jmin)
{
	if (direction() == Post3dWindowFaceSettingContainer::Direction::dirJ || ui->jMaxSlider->value() < jmin) {
		ui->jMaxSlider->blockSignals(true);
		ui->jMaxSlider->setValue(jmin);
		ui->jMaxSlider->blockSignals(false);
	}
}

void Post3dWindowFaceSettingEditWidget::jMaxChanged(int jmax)
{
	if (direction() == Post3dWindowFaceSettingContainer::Direction::dirJ || ui->jMinSlider->value() > jmax) {
		ui->jMinSlider->blockSignals(true);
		ui->jMinSlider->setValue(jmax);
		ui->jMinSlider->blockSignals(false);
	}
}

void Post3dWindowFaceSettingEditWidget::kMinChanged(int kmin)
{
	if (direction() == Post3dWindowFaceSettingContainer::Direction::dirK || ui->kMaxSlider->value() < kmin) {
		ui->kMaxSlider->blockSignals(true);
		ui->kMaxSlider->setValue(kmin);
		ui->kMaxSlider->blockSignals(false);
	}
}

void Post3dWindowFaceSettingEditWidget::kMaxChanged(int kmax)
{
	if (direction() == Post3dWindowFaceSettingContainer::Direction::dirK || ui->kMinSlider->value() > kmax) {
		ui->kMinSlider->blockSignals(true);
		ui->kMinSlider->setValue(kmax);
		ui->kMinSlider->blockSignals(false);
	}
}

void Post3dWindowFaceSettingEditWidget::iDirClicked()
{
	applyDirectionSetting();
	maximizeRange();
}

void Post3dWindowFaceSettingEditWidget::jDirClicked()
{
	applyDirectionSetting();
	maximizeRange();
}

void Post3dWindowFaceSettingEditWidget::kDirClicked()
{
	applyDirectionSetting();
	maximizeRange();
}

void Post3dWindowFaceSettingEditWidget::applyDirectionSetting()
{
	ui->iMaxSlider->setEnabled(true);
	ui->jMaxSlider->setEnabled(true);
	ui->kMaxSlider->setEnabled(true);

	switch (direction()) {
	case Post3dWindowFaceSettingContainer::Direction::dirI:
		ui->iMaxSlider->setDisabled(true);
		ui->iMaxSlider->setValue(ui->iMinSlider->value());
		break;

	case Post3dWindowFaceSettingContainer::Direction::dirJ:
		ui->jMaxSlider->setDisabled(true);
		ui->jMaxSlider->setValue(ui->jMinSlider->value());
		break;

	case Post3dWindowFaceSettingContainer::Direction::dirK:
		ui->kMaxSlider->setDisabled(true);
		ui->kMaxSlider->setValue(ui->kMinSlider->value());
		break;
	}
}

void Post3dWindowFaceSettingEditWidget::maximizeRange()
{
	switch (direction()) {
	case Post3dWindowFaceSettingContainer::Direction::dirI:
		ui->jMinSlider->setValue(ui->jMinSlider->minimum());
		ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
		ui->kMinSlider->setValue(ui->kMinSlider->minimum());
		ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
		break;

	case Post3dWindowFaceSettingContainer::Direction::dirJ:
		ui->iMinSlider->setValue(ui->iMinSlider->minimum());
		ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
		ui->kMinSlider->setValue(ui->kMinSlider->minimum());
		ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
		break;

	case Post3dWindowFaceSettingContainer::Direction::dirK:
		ui->iMinSlider->setValue(ui->iMinSlider->minimum());
		ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
		ui->jMinSlider->setValue(ui->jMinSlider->minimum());
		ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
		break;
	}
}

Post3dWindowFaceSettingContainer::Direction Post3dWindowFaceSettingEditWidget::direction() const
{
	if (ui->iRadioButton->isChecked()) {
		return Post3dWindowFaceSettingContainer::Direction::dirI;
	} else if (ui->jRadioButton->isChecked()) {
		return Post3dWindowFaceSettingContainer::Direction::dirJ;
	} else if (ui->kRadioButton->isChecked()) {
		return Post3dWindowFaceSettingContainer::Direction::dirK;
	}

	return Post3dWindowFaceSettingContainer::Direction::dirI; // to avoid compiler warning
}
