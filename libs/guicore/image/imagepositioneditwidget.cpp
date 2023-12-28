#include "imagepositioneditwidget.h"
#include "ui_imagepositioneditwidget.h"

ImagePositionEditWidget::ImagePositionEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ImagePositionEditWidget)
{
	ui->setupUi(this);
}

ImagePositionEditWidget::~ImagePositionEditWidget()
{
	delete ui;
}

ImageSettingContainer::Position ImagePositionEditWidget::position() const
{
	if (ui->topLeftRadioButton->isChecked()) {return ImageSettingContainer::Position::TopLeft;}
	if (ui->topRadioButton->isChecked()) {return ImageSettingContainer::Position::Top;}
	if (ui->topRightRadioButton->isChecked()) {return ImageSettingContainer::Position::TopRight;}
	if (ui->rightRadioButton->isChecked()) {return ImageSettingContainer::Position::Right;}
	if (ui->bottomRightRadioButton->isChecked()) {return ImageSettingContainer::Position::BottomRight;}
	if (ui->bottomRadioButton->isChecked()) {return ImageSettingContainer::Position::Bottom;}
	if (ui->bottomLeftRadioButton->isChecked()) {return ImageSettingContainer::Position::BottomLeft;}
	if (ui->leftRadioButton->isChecked()) {return ImageSettingContainer::Position::Left;}

	return ImageSettingContainer::Position::TopLeft;
}

void ImagePositionEditWidget::setPosition(ImageSettingContainer::Position pos)
{
	if (pos == ImageSettingContainer::Position::TopLeft) {
		ui->topLeftRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Top) {
		ui->topRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::TopRight) {
		ui->topRightRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Right) {
		ui->rightRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::BottomRight) {
		ui->bottomRightRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Bottom) {
		ui->bottomRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::BottomLeft) {
		ui->bottomLeftRadioButton->setChecked(true);
	} else if (pos == ImageSettingContainer::Position::Left) {
		ui->leftRadioButton->setChecked(true);
	}
}
