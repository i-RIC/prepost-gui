#include "geodatapointgroup_displaysettingwidget.h"
#include "../private/geodatapointgroup_impl.h"
#include "../private/geodatapointgroup_displaysetting.h"
#include "../private/geodatapointgroup_displaysettingwidget_scalesizepairtablecontroller.h"
#include "ui_geodatapointgroup_displaysettingwidget.h"

#include <misc/lastiodirectory.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <QFileDialog>

GeoDataPointGroup::DisplaySettingWidget::DisplaySettingWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	ui(new Ui::GeoDataPointGroup_DisplaySettingWidget)
{
	ui->setupUi(this);
	connect(ui->importImageButton, &QPushButton::clicked, [=](bool) {importImage();});
	connect(ui->clearImageButton, &QPushButton::clicked, [=](bool) {clearImage();});
	connect<void(QSpinBox::*)(int)>(ui->maxSizeSpinBox, &QSpinBox::valueChanged, [=](int) {updateImage();});
}

GeoDataPointGroup::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete ui;
}

QTableView* GeoDataPointGroup::DisplaySettingWidget::sizesTable() const
{
	return ui->sizesTableView;
}

QPushButton* GeoDataPointGroup::DisplaySettingWidget::removeButton() const
{
	return ui->removeButton;
}

QUndoCommand* GeoDataPointGroup::DisplaySettingWidget::createModifyCommand(bool /*apply*/)
{
	return new ValueModifyCommmand<DisplaySetting>(iRIC::generateCommandId("GeoDataPointGroup::PropertyDialog::Basic"), true, setting(), m_displaySetting);
}

GeoDataPointGroup::DisplaySetting GeoDataPointGroup::DisplaySettingWidget::setting() const
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
	setting.opacity = ui->transparencyWidget->opacity();
	setting.pointSize = ui->pointSizeSpinBox->value();

	auto pixmap = ui->imageLabel->pixmap();
	if (pixmap != nullptr) {
		setting.image = m_pixmap.toImage();
	}
	setting.imageMaxSize = ui->maxSizeSpinBox->value();

	return setting;
}

void GeoDataPointGroup::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	m_copySetting = setting;

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
	if (setting.anchorPosition == DisplaySetting::AnchorPosition::Center) {
		ui->anchorCenterRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::TopLeft) {
		ui->anchorTopLeftRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::Top) {
		ui->anchorTopRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::TopRight) {
		ui->anchorTopRightRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::Left) {
		ui->anchorLeftRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::Right) {
		ui->anchorRightRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::BottomLeft) {
		ui->anchorBottomLeftRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::Bottom) {
		ui->anchorBottomRadioButton->setChecked(true);
	} else if (setting.anchorPosition == DisplaySetting::AnchorPosition::BottomRight) {
		ui->anchorBottomRightRadioButton->setChecked(true);
	}

	ui->colorEditWidget->setColor(setting.color);
	ui->transparencyWidget->setOpacity(setting.opacity);
	ui->pointSizeSpinBox->setValue(setting.pointSize);
	if (! setting.image.isNull()) {
		m_pixmap = QPixmap::fromImage(setting.image);
	}
	ui->maxSizeSpinBox->setValue(setting.imageMaxSize);

	updateImage();

	m_scaleSizePairTableController->applyToTable();
}

void GeoDataPointGroup::DisplaySettingWidget::setSetting(DisplaySetting* setting)
{
	m_displaySetting = setting;
	setSetting(*setting);
}

void GeoDataPointGroup::DisplaySettingWidget::setIsReferenceInformation(bool isReference)
{
	if (! isReference) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->arbitraryRadioButton->setDisabled(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPointGroup::DisplaySettingWidget::importImage()
{
	auto fname = QFileDialog::getOpenFileName(this, tr("Select image file"), LastIODirectory::get(),
																						tr("All images(*.jpg *.jpeg *.png);;Jpeg images(*.jpg *.jpeg);;PNG images(*.png)"));
	if (fname.isNull()) {return;}

	QImage image(fname);
	m_pixmap = QPixmap::fromImage(image);

	updateImage();
}

void GeoDataPointGroup::DisplaySettingWidget::updateImage()
{
	if (m_pixmap.isNull()) {
		ui->imageLabel->setPixmap(QPixmap());
	}

	ui->imageLabel->setPixmap(Impl::shrinkPixmap(m_pixmap, ui->maxSizeSpinBox->value()));
}

void GeoDataPointGroup::DisplaySettingWidget::clearImage()
{
	m_pixmap = QPixmap();
	updateImage();
}

void GeoDataPointGroup::DisplaySettingWidget::addValue()
{
	int row = ui->sizesTableView->currentIndex().row();
	if (row == -1) {return;}

	auto& pairs = m_copySetting.scaleSizePairs;

	if (row == static_cast<int> (pairs.size()) - 1) {
		ScaleSizePair newPair;
		newPair.scale = pairs.at(row).scale * 2 - pairs.at(row - 1).scale;
		newPair.size = pairs.at(row).size * 2 - pairs.at(row - 1).size;
		pairs.push_back(newPair);
	} else {
		ScaleSizePair newPair;
		newPair.scale = (pairs.at(row).scale + pairs.at(row + 1).scale) * 0.5;
		newPair.size = (pairs.at(row).size + pairs.at(row + 1).size) * 0.5;
		pairs.insert(pairs.begin() + row + 1, newPair);
	}

	m_scaleSizePairTableController->applyToTable();
}

void GeoDataPointGroup::DisplaySettingWidget::removeValue()
{
	auto& pairs = m_copySetting.scaleSizePairs;
	std::vector<ScaleSizePair> newPairs;

	auto rows = ui->sizesTableView->selectionModel()->selectedRows();
	std::unordered_set<unsigned int> rowSet;
	for (auto r : rows) {
		rowSet.insert(r.row());
	}

	for (unsigned int i = 0; i < pairs.size(); ++i) {
		const auto& pair = pairs.at(i);
		if (rowSet.find(i) != rowSet.end()) {
			continue;
		}
		newPairs.push_back(pair);
	}

	pairs = newPairs;

	m_scaleSizePairTableController->applyToTable();
}
