#include "../image/imagepositioneditwidget.h"
#include "colormaplegendbulkeditwidget.h"
#include "colormaplegendsettingcontaineri.h"
#include "colormaplegendsettingmodifycommand.h"
#include "ui_colormaplegendbulkeditwidget.h"

#include <misc/opacitycontainer.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

ColorMapLegendBulkEditWidget::ColorMapLegendBulkEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	ui(new Ui::ColorMapLegendBulkEditWidget)
{
	ui->setupUi(this);
}

ColorMapLegendBulkEditWidget::~ColorMapLegendBulkEditWidget()
{
	delete ui;
}

void ColorMapLegendBulkEditWidget::setSettings(const std::vector<Setting> settings)
{
	m_settings.clear();

	for (const auto& s : settings) {
		m_settings.push_back(s.legendSetting);
	}

	auto area = ui->positionScrollArea;
	auto gLayout = new QGridLayout();

	int row = 0;
	for (const auto& s : settings) {
		auto label = new QLabel(s.title, area);
		gLayout->addWidget(label, row, 0);

		auto pos = new ImagePositionEditWidget(area);
		pos->setPosition(s.legendSetting->imgSetting()->position);
		gLayout->addWidget(pos, row, 1);
		m_widgets.push_back(pos);

		++ row;
	}
	auto hLayout = new QHBoxLayout();
	hLayout->addItem(gLayout);
	hLayout->setStretchFactor(gLayout, 0);
	hLayout->addStretch(1);

	auto vLayout = new QVBoxLayout();
	vLayout->addLayout(hLayout);
	vLayout->addStretch(1);
	vLayout->setStretchFactor(hLayout, 0);

	area->setLayout(vLayout);

	auto first = settings[0].legendSetting;

	auto dir = first->getDirection();
	if (dir == ColorMapLegendSettingContainerI::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	} else {
		ui->verticalRadioButton->setChecked(true);
	}
	ui->titleFontWidget->setFont(first->getTitleFont());
	ui->labelFontWidget->setFont(first->getLabelFont());

	ui->titleColorWidget->setColor(first->getTitleColor());
	ui->labelColorWidget->setColor(first->getLabelColor());
	ui->backgroundColorWidget->setColor(first->getBackgroundColor());
	ui->backgroundOpacityWidget->setOpacity(first->getBackgroundOpacity());
	ui->showBorderCheckBox->setChecked(first->getShowBorder());

	ui->horizontalMarginSpinBox->setValue(first->imgSetting()->horizontalMargin * 100);
	ui->verticalMarginSpinBox->setValue(first->imgSetting()->verticalMargin * 100);
	ui->widthSpinBox->setValue(first->imgSetting()->width);
	ui->heightSpinBox->setValue(first->imgSetting()->height);

	ui->barWidthAutoCheckBox->setChecked(first->getBarAutoWidth());
	ui->barWidthSpinBox->setValue(first->getBarWidth());

	auto align = first->getBarAlign();
	if (align == ColorMapLegendSettingContainerI::BarAlign::Left) {
		ui->leftRadioButton->setChecked(true);
	} else if (align == ColorMapLegendSettingContainerI::BarAlign::Center) {
		ui->centerRadioButton->setChecked(true);
	} else if (align == ColorMapLegendSettingContainerI::BarAlign::Right) {
		ui->rightRadioButton->setChecked(true);
	}
}

QUndoCommand* ColorMapLegendBulkEditWidget::createModifyCommand(bool apply)
{
	auto com = new MergeSupportedListCommand(iRIC::generateCommandId("ColorMapLegendBulkEditWidget"), apply);
	for (int i = 0; i < static_cast<int> (m_settings.size()); ++i) {
		auto setting = m_settings.at(i);
		auto pos = m_widgets.at(i);

		auto newSetting = setting->copy();
		newSetting->imgSetting()->position = pos->position();
		if (ui->horizontalRadioButton->isChecked()) {
			newSetting->setDirection(ColorMapLegendSettingContainerI::Direction::Horizontal);
		} else if (ui->verticalRadioButton->isChecked()) {
			newSetting->setDirection(ColorMapLegendSettingContainerI::Direction::Vertical);
		}

		newSetting->setTitleFont(ui->titleFontWidget->font());
		newSetting->setLabelFont(ui->labelFontWidget->font());

		newSetting->setTitleColor(ui->titleColorWidget->color());
		newSetting->setLabelColor(ui->labelColorWidget->color());
		newSetting->setBackgroundColor(ui->backgroundColorWidget->color());
		newSetting->setBackgroundOpacity(ui->backgroundOpacityWidget->opacity());
		newSetting->setShowBorder(ui->showBorderCheckBox->isChecked());

		newSetting->imgSetting()->horizontalMargin = ui->horizontalMarginSpinBox->value() / 100;
		newSetting->imgSetting()->verticalMargin = ui->verticalMarginSpinBox->value() / 100;
		newSetting->imgSetting()->width = ui->widthSpinBox->value();
		newSetting->imgSetting()->height = ui->heightSpinBox->value();

		newSetting->setBarAutoWidth(ui->barWidthAutoCheckBox->isChecked());
		newSetting->setBarWidth(ui->barWidthSpinBox->value());

		if (ui->leftRadioButton->isChecked()) {
			newSetting->setBarAlign(ColorMapLegendSettingContainerI::BarAlign::Left);
		} else if (ui->centerRadioButton->isChecked()) {
			newSetting->setBarAlign(ColorMapLegendSettingContainerI::BarAlign::Center);
		} else if (ui->rightRadioButton->isChecked()) {
			newSetting->setBarAlign(ColorMapLegendSettingContainerI::BarAlign::Right);
		}

		com->addCommand(new ColorMapLegendSettingModifyCommand(newSetting, setting));
	}

	return com;
}
