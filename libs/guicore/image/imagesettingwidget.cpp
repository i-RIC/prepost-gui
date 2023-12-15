#include "imagesettingcontainer.h"
#include "imagesettingwidget.h"
#include "public/imagesettingcontainer_imagebuilder.h"
#include "ui_imagesettingwidget.h"

ImageSettingWidget::ImageSettingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ImageSettingWidget)
{
	ui->setupUi(this);
	connect(ui->topLeftRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->topRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->topRightRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->leftRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->rightRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->bottomLeftRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->bottomRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
	connect(ui->bottomRightRadioButton, &QRadioButton::clicked, this, &ImageSettingWidget::handlePositionChange);
}

ImageSettingWidget::~ImageSettingWidget()
{
	delete ui;
}

ImageSettingContainer ImageSettingWidget::setting() const
{
	ImageSettingContainer ret;
	if (ui->topLeftRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::TopLeft;
	} else if (ui->topRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::Top;
	} else if (ui->topRightRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::TopRight;
	} else if (ui->leftRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::Left;
	} else if (ui->rightRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::Right;
	} else if (ui->bottomLeftRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::BottomLeft;
	} else if (ui->bottomRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::Bottom;
	} else if (ui->bottomRightRadioButton->isChecked()) {
		ret.position = ImageSettingContainer::Position::BottomRight;
	}

	ret.horizontalMargin = ui->horizontalMarginSpinBox->value() / 100;
	ret.verticalMargin = ui->verticalMarginSpinBox->value() / 100;
	ret.autoSize = ui->autoSizeCheckBox->isChecked();
	ret.width = ui->widthSpinBox->value();
	ret.height = ui->heightSpinBox->value();

	return ret;
}

void ImageSettingWidget::setSetting(const ImageSettingContainer& setting)
{
	auto pos = setting.position.value();
	if (pos == ImageSettingContainer::Position::TopLeft) {
		ui->topLeftRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Top) {
		ui->topRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::TopRight) {
		ui->topRightRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Left) {
		ui->leftRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Right) {
		ui->rightRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::BottomLeft) {
		ui->bottomLeftRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Bottom) {
		ui->bottomRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::BottomRight) {
		ui->bottomRightRadioButton->setChecked(true);
	}

	ui->horizontalMarginSpinBox->setValue(setting.horizontalMargin * 100);
	ui->verticalMarginSpinBox->setValue(setting.verticalMargin * 100);
	ui->autoSizeCheckBox->setChecked(setting.autoSize);

	if (setting.autoSize) {
		auto size = setting.imageBuilder()->autoSize();
		ui->widthSpinBox->setValue(size.width());
		ui->heightSpinBox->setValue(size.height());
	} else {
		ui->widthSpinBox->setValue(setting.width);
		ui->heightSpinBox->setValue(setting.height);
	}

	handlePositionChange();
}

int ImageSettingWidget::width() const
{
	return ui->widthSpinBox->value();
}

void ImageSettingWidget::setWidth(int width)
{
	ui->widthSpinBox->setValue(width);
}

int ImageSettingWidget::height() const
{
	return ui->heightSpinBox->value();
}

void ImageSettingWidget::setHeight(int height)
{
	ui->heightSpinBox->setValue(height);
}

void ImageSettingWidget::handlePositionChange()
{
	bool left =
			ui->topLeftRadioButton->isChecked() || ui->leftRadioButton->isChecked() || ui->bottomLeftRadioButton->isChecked();
	bool right =
			ui->topRightRadioButton->isChecked() || ui->rightRadioButton->isChecked() || ui->bottomRightRadioButton->isChecked();
	bool top =
			ui->topLeftRadioButton->isChecked() || ui->topRadioButton->isChecked() || ui->topRightRadioButton->isChecked();
	bool bottom =
			ui->bottomLeftRadioButton->isChecked() || ui->bottomRadioButton->isChecked() || ui->bottomRightRadioButton->isChecked();

	ui->horizontalMarginSpinBox->setEnabled(true);
	ui->verticalMarginSpinBox->setEnabled(true);

	if (left) {
		ui->horizontalMarginLabel->setText(tr("Distance from left"));
	} else if (right) {
		ui->horizontalMarginLabel->setText(tr("Distance from right"));
	} else {
		ui->horizontalMarginLabel->setText("");
		ui->horizontalMarginSpinBox->setDisabled(true);
	}

	if (top) {
		ui->verticalMarginLabel->setText(tr("Distance from top"));
	} else if (bottom) {
		ui->verticalMarginLabel->setText(tr("Distance from bottom"));
	} else {
		ui->verticalMarginLabel->setText("");
		ui->verticalMarginSpinBox->setDisabled(true);
	}
}
