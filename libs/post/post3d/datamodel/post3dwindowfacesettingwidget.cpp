#include "ui_post3dwindowfacesettingwidget.h"

#include "post3dwindowfacesettingwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkStructuredGrid.h>

Post3dWindowFaceSettingWidget::Post3dWindowFaceSettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowFaceSettingWidget)
{
	ui->setupUi(this);
	connect(ui->enabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(setMyEnabled(bool)));
	connect(ui->iRadioButton, SIGNAL(clicked()), this, SLOT(iDirClicked()));
	connect(ui->jRadioButton, SIGNAL(clicked()), this, SLOT(jDirClicked()));
	connect(ui->kRadioButton, SIGNAL(clicked()), this, SLOT(kDirClicked()));
	connect(ui->iMinSlider, SIGNAL(valueChanged(int)), this, SLOT(iMinChanged(int)));
	connect(ui->iMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(iMaxChanged(int)));
	connect(ui->jMinSlider, SIGNAL(valueChanged(int)), this, SLOT(jMinChanged(int)));
	connect(ui->jMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(jMaxChanged(int)));
	connect(ui->kMinSlider, SIGNAL(valueChanged(int)), this, SLOT(kMinChanged(int)));
	connect(ui->kMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(kMaxChanged(int)));
	ui->enabledCheckBox->setChecked(true);
	ui->enabledCheckBox->setChecked(false);
	m_multiSelected = false;
}

Post3dWindowFaceSettingWidget::~Post3dWindowFaceSettingWidget()
{
	delete ui;
}

void Post3dWindowFaceSettingWidget::setZoneData(PostZoneDataContainer* zd)
{
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(zd->data());
	int dims[3];
	grid->GetDimensions(dims);
	ui->iMinSlider->setRange(1, dims[0]);
	ui->iMaxSlider->setRange(1, dims[0]);
	ui->jMinSlider->setRange(1, dims[1]);
	ui->jMaxSlider->setRange(1, dims[1]);
	ui->kMinSlider->setRange(1, dims[2]);
	ui->kMaxSlider->setRange(1, dims[2]);
}

void Post3dWindowFaceSettingWidget::setSetting(const Post3dWindowFaceDataItem::Setting& setting)
{
	m_setting = setting;
	ui->enabledCheckBox->setChecked(m_setting.enabled);
	setMyEnabled(m_setting.enabled);
	applyDirectionSetting();
	applySliderValues();
}


void Post3dWindowFaceSettingWidget::setMyEnabled(bool enabled)
{
	m_setting.enabled = enabled;
	ui->directionGroupBox->setEnabled(enabled);
	ui->rangeGroupBox->setEnabled(enabled);
	if (enabled) {
		applyDirectionSetting();
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::sliderRange(int range[6])
{
	range[0] = ui->iMinSlider->minimum() - 1;
	range[1] = ui->iMaxSlider->maximum() - 1;
	range[2] = ui->jMinSlider->minimum() - 1;
	range[3] = ui->jMaxSlider->maximum() - 1;
	range[4] = ui->kMinSlider->minimum() - 1;
	range[5] = ui->kMaxSlider->maximum() - 1;
}

void Post3dWindowFaceSettingWidget::iMinChanged(int imin)
{
	m_setting.iMin = imin - 1;
	if (m_setting.direction == Post3dWindowFaceDataItem::dirI || ui->iMaxSlider->value() < imin) {
		ui->iMaxSlider->blockSignals(true);
		ui->iMaxSlider->setValue(imin);
		ui->iMaxSlider->blockSignals(false);
		m_setting.iMax = imin - 1;
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::iMaxChanged(int imax)
{
	m_setting.iMax = imax - 1;
	if (ui->iMinSlider->value() > imax) {
		ui->iMinSlider->blockSignals(true);
		ui->iMinSlider->setValue(imax);
		ui->iMinSlider->blockSignals(false);
		m_setting.iMin = imax - 1;
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::jMinChanged(int jmin)
{
	m_setting.jMin = jmin - 1;
	if (m_setting.direction == Post3dWindowFaceDataItem::dirJ || ui->jMaxSlider->value() < jmin) {
		ui->jMaxSlider->blockSignals(true);
		ui->jMaxSlider->setValue(jmin);
		ui->jMaxSlider->blockSignals(false);
		m_setting.jMax = jmin - 1;
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::jMaxChanged(int jmax)
{
	m_setting.jMax = jmax - 1;
	if (ui->jMinSlider->value() > jmax) {
		ui->jMinSlider->blockSignals(true);
		ui->jMinSlider->setValue(jmax);
		ui->jMinSlider->blockSignals(false);
		m_setting.jMin = jmax - 1;
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::kMinChanged(int kmin)
{
	m_setting.kMin = kmin - 1;
	if (m_setting.direction == Post3dWindowFaceDataItem::dirK || ui->kMaxSlider->value() < kmin) {
		ui->kMaxSlider->blockSignals(true);
		ui->kMaxSlider->setValue(kmin);
		ui->kMaxSlider->blockSignals(false);
		m_setting.kMax = kmin - 1;
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::kMaxChanged(int kmax)
{
	m_setting.kMax = kmax - 1;
	if (ui->kMinSlider->value() > kmax) {
		ui->kMinSlider->blockSignals(true);
		ui->kMinSlider->setValue(kmax);
		ui->kMinSlider->blockSignals(false);
		m_setting.kMin = kmax - 1;
	}
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::iDirClicked()
{
	m_setting.direction = Post3dWindowFaceDataItem::dirI;
	applyDirectionSetting();
	maximizeRange();
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::jDirClicked()
{
	m_setting.direction = Post3dWindowFaceDataItem::dirJ;
	applyDirectionSetting();
	maximizeRange();
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::kDirClicked()
{
	m_setting.direction = Post3dWindowFaceDataItem::dirK;
	applyDirectionSetting();
	maximizeRange();
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::applyDirectionSetting()
{
	ui->iMaxSlider->setEnabled(true);
	ui->jMaxSlider->setEnabled(true);
	ui->kMaxSlider->setEnabled(true);
	switch (m_setting.direction) {
	case Post3dWindowFaceDataItem::dirI:
		ui->iRadioButton->setChecked(true);
		ui->iMaxSlider->setDisabled(true);
		ui->iMaxSlider->setValue(ui->iMinSlider->value());
		break;
	case Post3dWindowFaceDataItem::dirJ:
		ui->jRadioButton->setChecked(true);
		ui->jMaxSlider->setDisabled(true);
		ui->jMaxSlider->setValue(ui->jMinSlider->value());
		break;
	case Post3dWindowFaceDataItem::dirK:
		ui->kRadioButton->setChecked(true);
		ui->kMaxSlider->setDisabled(true);
		ui->kMaxSlider->setValue(ui->kMinSlider->value());
		break;
	}
}

void Post3dWindowFaceSettingWidget::maximizeRange()
{
	switch (m_setting.direction) {
	case Post3dWindowFaceDataItem::dirI:
		ui->jMinSlider->setValue(ui->jMinSlider->minimum());
		ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
		ui->kMinSlider->setValue(ui->kMinSlider->minimum());
		ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
		break;
	case Post3dWindowFaceDataItem::dirJ:
		ui->iMinSlider->setValue(ui->iMinSlider->minimum());
		ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
		ui->kMinSlider->setValue(ui->kMinSlider->minimum());
		ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
		break;
	case Post3dWindowFaceDataItem::dirK:
		ui->iMinSlider->setValue(ui->iMinSlider->minimum());
		ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
		ui->jMinSlider->setValue(ui->jMinSlider->minimum());
		ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
		break;
	}
}

void Post3dWindowFaceSettingWidget::applySliderValues()
{
	ui->iMinSlider->blockSignals(true);
	ui->iMaxSlider->blockSignals(true);
	ui->jMinSlider->blockSignals(true);
	ui->jMaxSlider->blockSignals(true);
	ui->kMinSlider->blockSignals(true);
	ui->kMaxSlider->blockSignals(true);

	ui->iMinSlider->setValue(m_setting.iMin + 1);
	ui->iMaxSlider->setValue(m_setting.iMax + 1);
	ui->jMinSlider->setValue(m_setting.jMin + 1);
	ui->jMaxSlider->setValue(m_setting.jMax + 1);
	ui->kMinSlider->setValue(m_setting.kMin + 1);
	ui->kMaxSlider->setValue(m_setting.kMax + 1);

	switch (m_setting.direction) {
	case Post3dWindowFaceDataItem::dirI:
		ui->iMaxSlider->setValue(ui->iMinSlider->value());
		break;
	case Post3dWindowFaceDataItem::dirJ:
		ui->jMaxSlider->setValue(ui->jMinSlider->value());
		break;
	case Post3dWindowFaceDataItem::dirK:
		ui->kMaxSlider->setValue(ui->kMinSlider->value());
		break;
	}

	ui->iMinSlider->blockSignals(false);
	ui->iMaxSlider->blockSignals(false);
	ui->jMinSlider->blockSignals(false);
	ui->jMaxSlider->blockSignals(false);
	ui->kMinSlider->blockSignals(false);
	ui->kMaxSlider->blockSignals(false);
}

void Post3dWindowFaceSettingWidget::setEnabledCheck(bool check)
{
	m_setting.enabled = check;
	ui->enabledCheckBox->setChecked(check);
	emit settingChanged();
}

void Post3dWindowFaceSettingWidget::setMultiSelected(bool selected)
{
	m_multiSelected = selected;
}
