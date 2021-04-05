#include "post3dfacesettingcontainer.h"
#include "post3dsamplingratesettingcontainer.h"
#include "post3dwindownodegrouparrowfacesettingwidget.h"

#include "ui_post3dwindownodegrouparrowfacesettingwidget.h"

#include <guibase/arrowsettingcontainer.h>
#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post3dWindowNodeGroupArrowFaceSettingWidget::Post3dWindowNodeGroupArrowFaceSettingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowNodeGroupArrowFaceSettingWidget)
{
	ui->setupUi(this);

	connect(ui->iMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowNodeGroupArrowFaceSettingWidget::iMinChanged);
	connect(ui->iMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowNodeGroupArrowFaceSettingWidget::iMaxChanged);
	connect(ui->jMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowNodeGroupArrowFaceSettingWidget::jMinChanged);
	connect(ui->jMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowNodeGroupArrowFaceSettingWidget::jMaxChanged);
	connect(ui->kMinSlider, &SliderWithValue::valueChanged, this, &Post3dWindowNodeGroupArrowFaceSettingWidget::kMinChanged);
	connect(ui->kMaxSlider, &SliderWithValue::valueChanged, this, &Post3dWindowNodeGroupArrowFaceSettingWidget::kMaxChanged);

	connect(ui->iRadioButton, SIGNAL(clicked()), this, SLOT(iDirClicked()));
	connect(ui->jRadioButton, SIGNAL(clicked()), this, SLOT(jDirClicked()));
	connect(ui->kRadioButton, SIGNAL(clicked()), this, SLOT(kDirClicked()));
}

Post3dWindowNodeGroupArrowFaceSettingWidget::~Post3dWindowNodeGroupArrowFaceSettingWidget()
{
	delete ui;
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::setZoneData(PostZoneDataContainer* zd)
{
	auto grid = dynamic_cast<vtkStructuredGrid*> (zd->data());
	int dims[3];
	grid->GetDimensions(dims);

	ui->iMinSlider->setRange(1, dims[0]);
	ui->iMaxSlider->setRange(1, dims[0]);
	ui->jMinSlider->setRange(1, dims[1]);
	ui->jMaxSlider->setRange(1, dims[1]);
	ui->kMinSlider->setRange(1, dims[2]);
	ui->kMaxSlider->setRange(1, dims[2]);

	auto gType = zd->gridType();
	auto pd = zd->data()->GetPointData();

	m_scalars = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_scalars), ui->scalarComboBox);

	if (m_scalars.size() == 0) {
		ui->specificRadioButton->setChecked(true);
		ui->scalarRadioButton->setDisabled(true);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::setSettings(bool enabled, const ArrowSettingContainer& arrowSetting, const Post3dFaceSettingContainer& faceSetting, const Post3dSamplingRateSettingContainer& samplingRateSetting)
{
	ui->enabledCheckBox->setChecked(enabled);

	applyFaceSetting(faceSetting);
	applyDirectionSetting(faceSetting);

	if (arrowSetting.colorMode == ArrowSettingContainer::ColorMode::Custom) {
		ui->specificRadioButton->setChecked(true);
	} else if (arrowSetting.colorMode == ArrowSettingContainer::ColorMode::ByScalar) {
		ui->scalarRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(arrowSetting.customColor);

	ui->arrowSizeSpinBox->setValue(arrowSetting.arrowSize);
	ui->lineWidthSpinBox->setValue(arrowSetting.lineWidth);

	if (arrowSetting.samplingMode == ArrowSettingContainer::SamplingMode::All) {
		ui->samplingAllRadioButton->setChecked(true);
	} else {
		ui->samplingSkipRadioButton->setChecked(true);
	}

	ui->iSamplingRateSpinBox->setValue(samplingRateSetting.iSamplingRate);
	ui->jSamplingRateSpinBox->setValue(samplingRateSetting.jSamplingRate);
	ui->kSamplingRateSpinBox->setValue(samplingRateSetting.kSamplingRate);
}

bool Post3dWindowNodeGroupArrowFaceSettingWidget::enabled() const
{
	return ui->enabledCheckBox->isChecked();
}

ArrowSettingContainer Post3dWindowNodeGroupArrowFaceSettingWidget::arrowSetting() const
{
	ArrowSettingContainer ret;

	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::All;
	} else {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::Rate;
	}

	if (ui->specificRadioButton->isChecked()) {
		ret.colorMode = ArrowSettingContainer::ColorMode::Custom;
		ret.customColor = ui->colorEditWidget->color();
	} else {
		ret.colorMode = ArrowSettingContainer::ColorMode::ByScalar;
		ret.colorTarget = m_scalars.at(ui->scalarComboBox->currentIndex()).c_str();
	}
	ret.arrowSize = ui->arrowSizeSpinBox->value();
	ret.lineWidth = ui->lineWidthSpinBox->value();

	return ret;
}

Post3dFaceSettingContainer Post3dWindowNodeGroupArrowFaceSettingWidget::faceSetting() const
{
	Post3dFaceSettingContainer ret;
	if (ui->iRadioButton->isChecked()) {
		ret.direction = Post3dFaceSettingContainer::Direction::dirI;
	}	else if (ui->jRadioButton->isChecked()) {
		ret.direction = Post3dFaceSettingContainer::Direction::dirJ;
	}	else if (ui->kRadioButton->isChecked()) {
		ret.direction = Post3dFaceSettingContainer::Direction::dirK;
	}

	ret.iMin = ui->iMinSlider->value() - 1;
	ret.iMax = ui->iMaxSlider->value() - 1;
	ret.jMin = ui->jMinSlider->value() - 1;
	ret.jMax = ui->jMaxSlider->value() - 1;
	ret.kMin = ui->kMinSlider->value() - 1;
	ret.kMax = ui->kMaxSlider->value() - 1;

	return ret;
}

Post3dSamplingRateSettingContainer Post3dWindowNodeGroupArrowFaceSettingWidget::samplingRateSetting() const
{
	Post3dSamplingRateSettingContainer ret;

	if (ui->samplingAllRadioButton->isChecked()) {
		ret.iSamplingRate = 1;
		ret.jSamplingRate = 1;
		ret.kSamplingRate = 1;
	} else {
		ret.iSamplingRate = ui->iSamplingRateSpinBox->value();
		ret.jSamplingRate = ui->jSamplingRateSpinBox->value();
		ret.kSamplingRate = ui->kSamplingRateSpinBox->value();
	}

	return ret;
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::iMinChanged(int imin)
{
	if (ui->iRadioButton->isChecked() || imin > ui->iMaxSlider->value()) {
		ui->iMaxSlider->setValue(imin);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::iMaxChanged(int imax)
{
	if (ui->iRadioButton->isChecked() || imax < ui->iMinSlider->value()) {
		ui->iMinSlider->setValue(imax);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::jMinChanged(int jmin)
{
	if (ui->jRadioButton->isChecked() || jmin > ui->jMaxSlider->value()) {
		ui->jMaxSlider->setValue(jmin);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::jMaxChanged(int jmax)
{
	if (ui->jRadioButton->isChecked() || jmax < ui->jMinSlider->value()) {
		ui->jMinSlider->setValue(jmax);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::kMinChanged(int kmin)
{
	if (ui->kRadioButton->isChecked() || kmin > ui->kMaxSlider->value()) {
		ui->kMaxSlider->setValue(kmin);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::kMaxChanged(int kmax)
{
	if (ui->kRadioButton->isChecked() || kmax < ui->kMinSlider->value()) {
		ui->kMinSlider->setValue(kmax);
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::iDirClicked()
{
	Post3dFaceSettingContainer s = faceSetting();
	s.direction = Post3dFaceSettingContainer::Direction::dirI;
	applyDirectionSetting(s);
	maximizeRange();
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::jDirClicked()
{
	Post3dFaceSettingContainer s = faceSetting();
	s.direction = Post3dFaceSettingContainer::Direction::dirJ;
	applyDirectionSetting(s);
	maximizeRange();
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::kDirClicked()
{
	Post3dFaceSettingContainer s = faceSetting();
	s.direction = Post3dFaceSettingContainer::Direction::dirK;
	applyDirectionSetting(s);
	maximizeRange();
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::applyDirectionSetting(const Post3dFaceSettingContainer& fs)
{
	ui->iMaxSlider->setEnabled(true);
	ui->jMaxSlider->setEnabled(true);
	ui->kMaxSlider->setEnabled(true);

	switch (fs.direction.value()) {
	case Post3dFaceSettingContainer::Direction::dirI:
		ui->iRadioButton->setChecked(true);
		ui->iMaxSlider->setDisabled(true);
		ui->iMaxSlider->setValue(ui->iMinSlider->value());
		break;
	case Post3dFaceSettingContainer::Direction::dirJ:
		ui->jRadioButton->setChecked(true);
		ui->jMaxSlider->setDisabled(true);
		ui->jMaxSlider->setValue(ui->jMinSlider->value());
		break;
	case Post3dFaceSettingContainer::Direction::dirK:
		ui->kRadioButton->setChecked(true);
		ui->kMaxSlider->setDisabled(true);
		ui->kMaxSlider->setValue(ui->kMinSlider->value());
		break;
	}
}

void Post3dWindowNodeGroupArrowFaceSettingWidget::applyFaceSetting(const Post3dFaceSettingContainer& fs)
{
	ui->iMinSlider->blockSignals(true);
	ui->iMaxSlider->blockSignals(true);
	ui->jMinSlider->blockSignals(true);
	ui->jMaxSlider->blockSignals(true);
	ui->kMinSlider->blockSignals(true);
	ui->kMaxSlider->blockSignals(true);

	ui->iMinSlider->setValue(fs.iMin + 1);
	ui->iMaxSlider->setValue(fs.iMax + 1);
	ui->jMinSlider->setValue(fs.jMin + 1);
	ui->jMaxSlider->setValue(fs.jMax + 1);
	ui->kMinSlider->setValue(fs.kMin + 1);
	ui->kMaxSlider->setValue(fs.kMax + 1);

	switch (fs.direction.value()) {
	case Post3dFaceSettingContainer::Direction::dirI:
		ui->iRadioButton->setChecked(true);
		ui->iMaxSlider->setValue(ui->iMinSlider->value());
		break;
	case Post3dFaceSettingContainer::Direction::dirJ:
		ui->jRadioButton->setChecked(true);
		ui->jMaxSlider->setValue(ui->jMinSlider->value());
		break;
	case Post3dFaceSettingContainer::Direction::dirK:
		ui->kRadioButton->setChecked(true);
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

void Post3dWindowNodeGroupArrowFaceSettingWidget::maximizeRange()
{
	if (ui->iRadioButton->isChecked()) {
		ui->jMinSlider->setValue(ui->jMinSlider->minimum());
		ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
		ui->kMinSlider->setValue(ui->kMinSlider->minimum());
		ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
	} else if (ui->jRadioButton->isChecked()) {
		ui->iMinSlider->setValue(ui->iMinSlider->minimum());
		ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
		ui->kMinSlider->setValue(ui->kMinSlider->minimum());
		ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
	} else if (ui->kRadioButton->isChecked()) {
		ui->iMinSlider->setValue(ui->iMinSlider->minimum());
		ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
		ui->jMinSlider->setValue(ui->jMinSlider->minimum());
		ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
	}
}
