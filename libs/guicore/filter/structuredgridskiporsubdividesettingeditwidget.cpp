#include "structuredgridskiporsubdividesettingcontainer.h"
#include "structuredgridskiporsubdividesettingeditwidget.h"
#include "ui_structuredgridskiporsubdividesettingeditwidget.h"

StructuredGridSkipOrSubdivideSettingEditWidget::StructuredGridSkipOrSubdivideSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::StructuredGridSkipOrSubdivideSettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->spaceSlider, &QSlider::valueChanged, this, &StructuredGridSkipOrSubdivideSettingEditWidget::handleSpaceSliderChange);

	setupNominations(5);
	ui->spaceSlider->setTracking(true);
}

StructuredGridSkipOrSubdivideSettingEditWidget::~StructuredGridSkipOrSubdivideSettingEditWidget()
{
	delete ui;
}

void StructuredGridSkipOrSubdivideSettingEditWidget::setupNominations(int max)
{
	for (int n = 2; n <= max; ++n) {
		m_subDivideNominations.push_back(n);
		m_skipNominations.push_back(n);
	}
	ui->spaceSlider->setMaximum(static_cast<int>(m_subDivideNominations.size() + m_skipNominations.size()));
	ui->spaceMinLabel->setText(QString("1/%1").arg(max));
	ui->spaceMaxLabel->setText(QString("%1").arg(max));
}

StructuredGridSkipOrSubdivideSettingContainer StructuredGridSkipOrSubdivideSettingEditWidget::setting() const
{
	StructuredGridSkipOrSubdivideSettingContainer ret;
	int v = ui->spaceSlider->value();
	if (v < static_cast<int> (m_skipNominations.size())) {
		ret.samplingMode = StructuredGridSkipOrSubdivideSettingContainer::SamplingMode::Skip;
		ret.skipRate = m_skipNominations.at(m_skipNominations.size() - ui->spaceSlider->value() - 1);
	} else if (v > static_cast<int> (m_skipNominations.size())) {
		ret.samplingMode = StructuredGridSkipOrSubdivideSettingContainer::SamplingMode::Subdivide;
		ret.subDivideRate = m_subDivideNominations.at(ui->spaceSlider->value() - m_skipNominations.size() - 1);
	} else {
		ret.samplingMode = StructuredGridSkipOrSubdivideSettingContainer::SamplingMode::Normal;
	}

	return ret;
}

void StructuredGridSkipOrSubdivideSettingEditWidget::setSetting(const StructuredGridSkipOrSubdivideSettingContainer& setting)
{
	ui->spaceSlider->setValue(static_cast<int> (m_skipNominations.size()));
	if (setting.samplingMode == StructuredGridSkipOrSubdivideSettingContainer::SamplingMode::Normal) {
		// do nothing
	} else if (setting.samplingMode == StructuredGridSkipOrSubdivideSettingContainer::SamplingMode::Skip) {
		for (int i = 0; i < static_cast<int>(m_skipNominations.size()); ++i) {
			if (m_skipNominations.at(i) == setting.skipRate) {
				ui->spaceSlider->setValue(static_cast<int> (m_skipNominations.size()) - i - 1);
			}
		}
	} else if (setting.samplingMode == StructuredGridSkipOrSubdivideSettingContainer::SamplingMode::Subdivide) {
		for (int i = 0; i < static_cast<int>(m_subDivideNominations.size()); ++i) {
			if (m_subDivideNominations.at(i) == setting.subDivideRate) {
				ui->spaceSlider->setValue(static_cast<int> (m_skipNominations.size() + i + 1));
			}
		}
	}
}

void StructuredGridSkipOrSubdivideSettingEditWidget::handleSpaceSliderChange(int val)
{
	if (val < static_cast<int>(m_skipNominations.size())) {
		ui->spaceValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(m_skipNominations.size() - val - 1)));
	} else if (val > static_cast<int>(m_skipNominations.size())) {
		ui->spaceValueLabel->setText(QString("%1").arg(m_subDivideNominations.at(val - m_skipNominations.size() - 1)));
	} else {
		ui->spaceValueLabel->setText("1");
	}
}
