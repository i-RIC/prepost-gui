#include "post2dwindowparticleimagedataitem_impl.h"
#include "post2dwindowparticleimagedataitem_setting.h"
#include "post2dwindowparticleimagedataitem_settingeditwidget.h"
#include "post2dwindowparticleimagedataitem_settingeditwidget_valuesizepairtablecontroller.h"
#include "ui_post2dwindowparticleimagedataitem_settingeditwidget.h"

#include <misc/lastiodirectory.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowParticleImageDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowParticleImageDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::Post2dWindowParticleImageDataItem_SettingEditWidget),
	m_valueSizePairTableController {nullptr}
{
	ui->setupUi(this);

	connect(ui->importImageButton, &QPushButton::clicked, this, &SettingEditWidget::importImage);
	connect(ui->clearImageButton, &QPushButton::clicked, this, &SettingEditWidget::clearImage);
	connect(ui->addButton, &QPushButton::clicked, this, &SettingEditWidget::addValue);
	connect(ui->removeButton, &QPushButton::clicked, this, &SettingEditWidget::removeValue);

	m_valueSizePairTableController = new ValueSizePairTableController(this);

	m_setting = item->impl->m_setting;
	setSetting(m_setting);
}

Post2dWindowParticleImageDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete m_valueSizePairTableController;
	delete ui;
}

QTableView* Post2dWindowParticleImageDataItem::SettingEditWidget::sizesTable() const
{
	return ui->sizesTableView;
}

QPushButton* Post2dWindowParticleImageDataItem::SettingEditWidget::removeButton() const
{
	return ui->removeButton;
}

QUndoCommand* Post2dWindowParticleImageDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Post2dWindowParticleImageDataItem::SettingEditWidget"), apply,  setting(), &m_item->impl->m_setting);
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->imageLabel->setPixmap(QPixmap::fromImage(setting.image));
	updateWidthAndHeight();

	if (setting.anchorPosition == Setting::AnchorPosition::Center) {
		ui->anchorCenterRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::TopLeft) {
		ui->anchorTopLeftRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::Top) {
		ui->anchorTopRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::TopRight) {
		ui->anchorTopRightRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::Left) {
		ui->anchorLeftRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::Right) {
		ui->anchorRightRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::BottomLeft) {
		ui->anchorBottomLeftRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::Bottom) {
		ui->anchorBottomRadioButton->setChecked(true);
	} else if (setting.anchorPosition == Setting::AnchorPosition::BottomRight) {
		ui->anchorBottomRightRadioButton->setChecked(true);
	}

	if (setting.sizeTarget == Setting::SizeTarget::Width) {
		ui->widthRadioButton->setChecked(true);
	} else if (setting.sizeTarget == Setting::SizeTarget::Height) {
		ui->heightRadioButton->setChecked(true);
	}
	ui->fillUpperCheckBox->setChecked(setting.fillUpper);
	ui->fillLowerCheckBox->setChecked(setting.fillLower);

	ui->opacityWidget->setOpacity(setting.opacity);

	m_valueSizePairTableController->applyToTable();
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::importImage()
{
	auto fname = QFileDialog::getOpenFileName(this, tr("Select image file"), LastIODirectory::get(),
																						tr("All images(*.jpg *.jpeg *.png);;Jpeg images(*.jpg *.jpeg);;PNG images(*.png)"));
	if (fname.isNull()) {return;}

	QImage image(fname);
	ui->imageLabel->setPixmap(QPixmap::fromImage(image));
	updateWidthAndHeight();
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::clearImage()
{
	QImage image(":/libs/post/post2d/images/particleImage.png");

	ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::addValue()
{
	int row = ui->sizesTableView->currentIndex().row();
	if (row == -1) {return;}

	auto& pairs = m_setting.valueSizePairs;

	if (row == static_cast<int> (pairs.size()) - 1) {
		ValueSizePair newPair;
		newPair.value = pairs.at(row).value * 2 - pairs.at(row - 1).value;
		newPair.size = pairs.at(row).size * 2 - pairs.at(row - 1).size;
		pairs.push_back(newPair);
	} else {
		ValueSizePair newPair;
		newPair.value = (pairs.at(row).value + pairs.at(row + 1).value) * 0.5;
		newPair.size = (pairs.at(row).size + pairs.at(row + 1).size) * 0.5;
		pairs.insert(pairs.begin() + row + 1, newPair);
	}

	m_valueSizePairTableController->applyToTable();
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::removeValue()
{
	auto& pairs = m_setting.valueSizePairs;
	std::vector<ValueSizePair> newPairs;

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

	m_valueSizePairTableController->applyToTable();
}

Post2dWindowParticleImageDataItem::Setting Post2dWindowParticleImageDataItem::SettingEditWidget::setting() const
{
	Setting setting = m_setting;
	setting.image = ui->imageLabel->pixmap()->toImage();

	if (ui->anchorCenterRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::Center;
	} else if (ui->anchorTopLeftRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::TopLeft;
	} else if (ui->anchorTopRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::Top;
	} else if (ui->anchorTopRightRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::TopRight;
	} else if (ui->anchorLeftRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::Left;
	} else if (ui->anchorRightRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::Right;
	} else if (ui->anchorBottomLeftRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::BottomLeft;
	} else if (ui->anchorBottomRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::Bottom;
	} else if (ui->anchorBottomRightRadioButton->isChecked()) {
		setting.anchorPosition = Setting::AnchorPosition::BottomRight;
	}

	if (ui->widthRadioButton->isChecked()) {
		setting.sizeTarget = Setting::SizeTarget::Width;
	} else if (ui->heightRadioButton->isChecked()) {
		setting.sizeTarget = Setting::SizeTarget::Height;
	}
	setting.fillUpper = ui->fillUpperCheckBox->isChecked();
	setting.fillLower = ui->fillLowerCheckBox->isChecked();

	setting.opacity = ui->opacityWidget->opacity();

	return setting;
}

void Post2dWindowParticleImageDataItem::SettingEditWidget::updateWidthAndHeight()
{
	auto pixmap = ui->imageLabel->pixmap();
	if (pixmap == nullptr || pixmap->isNull()) {
		ui->widthValueLabel->setText("---");
		ui->heightValueLabel->setText("---");
	} else {
		ui->widthValueLabel->setText(QString::number(pixmap->width()));
		ui->heightValueLabel->setText(QString::number(pixmap->height()));
	}
}
