#include "geodatapoint_displaysettingwidget.h"
#include "../private/geodatapoint_impl.h"
#include "../private/geodatapoint_displaysetting.h"
#include "ui_geodatapoint_displaysettingwidget.h"

#include <misc/lastiodirectory.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <QFileDialog>

GeoDataPoint::DisplaySettingWidget::DisplaySettingWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	ui(new Ui::GeoDataPoint_DisplaySettingWidget)
{
	ui->setupUi(this);
	connect(ui->importImageButton, &QPushButton::clicked, [=](bool) {importImage();});
	connect(ui->clearImageButton, &QPushButton::clicked, [=](bool) {clearImage();});
	connect<void(QSpinBox::*)(int)>(ui->maxSizeSpinBox, &QSpinBox::valueChanged, [=](int) {updateImage();});
}

GeoDataPoint::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete ui;
}

QUndoCommand* GeoDataPoint::DisplaySettingWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<DisplaySetting>(iRIC::generateCommandId("GeoDataPoint::PropertyDialog::Basic"), true, setting(), m_displaySetting);
}

GeoDataPoint::DisplaySetting GeoDataPoint::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;

	if (ui->arbitraryRadioButton->isChecked()) {
		setting.mapping = DisplaySetting::Mapping::Arbitrary;
	} else if (ui->byValueRadioButton->isChecked()) {
		setting.mapping = DisplaySetting::Mapping::Value;
	}
	if (ui->pointsRadioButton->isChecked()) {
		setting.shape = DisplaySetting::Shape::Point;
	} else if (ui->imagesRadioButton->isChecked()) {
		setting.shape = DisplaySetting::Shape::Image;
	}
	setting.color = ui->colorEditWidget->color();
	setting.opacity = ui->transparencyWidget->opacityPercent();
	setting.pointSize = ui->pointSizeSpinBox->value();

	auto pixmap = ui->imageLabel->pixmap();
	if (pixmap != nullptr) {
		setting.image = m_pixmap.toImage();
	}
	setting.imageMaxSize = ui->maxSizeSpinBox->value();

	return setting;
}

void GeoDataPoint::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	if (setting.mapping == DisplaySetting::Mapping::Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	} else if (setting.mapping == DisplaySetting::Mapping::Value) {
		ui->byValueRadioButton->setChecked(true);
	}
	if (setting.shape == DisplaySetting::Shape::Point) {
		ui->pointsRadioButton->setChecked(true);
	} else if (setting.shape == DisplaySetting::Shape::Image) {
		ui->imagesRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(setting.color);
	ui->transparencyWidget->setOpacityPercent(setting.opacity);
	ui->pointSizeSpinBox->setValue(setting.pointSize);
	if (! setting.image.isNull()) {
		m_pixmap = QPixmap::fromImage(setting.image);
	}
	ui->maxSizeSpinBox->setValue(setting.imageMaxSize);

	updateImage();
}

void GeoDataPoint::DisplaySettingWidget::setSetting(DisplaySetting* setting)
{
	m_displaySetting = setting;
	setSetting(*setting);
}

void GeoDataPoint::DisplaySettingWidget::setIsReferenceInformation(bool isReference)
{
	if (! isReference) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->arbitraryRadioButton->setDisabled(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPoint::DisplaySettingWidget::importImage()
{
	auto fname = QFileDialog::getOpenFileName(this, tr("Select image file"), LastIODirectory::get(),
																						tr("All images(*.jpg *.jpeg *.png);;Jpeg images(*.jpg *.jpeg);;PNG images(*.png)"));
	if (fname.isNull()) {return;}

	QImage image(fname);
	m_pixmap = QPixmap::fromImage(image);

	updateImage();
}

void GeoDataPoint::DisplaySettingWidget::updateImage()
{
	if (m_pixmap.isNull()) {
		ui->imageLabel->setPixmap(QPixmap());
	}

	ui->imageLabel->setPixmap(Impl::shrinkPixmap(m_pixmap, ui->maxSizeSpinBox->value()));
}

void GeoDataPoint::DisplaySettingWidget::clearImage()
{
	m_pixmap = QPixmap();
	updateImage();
}
