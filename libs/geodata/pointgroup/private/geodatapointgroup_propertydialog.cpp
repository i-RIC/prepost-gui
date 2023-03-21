#include "geodatapointgroup_impl.h"
#include "geodatapointgroup_propertydialog.h"
#include "geodatapointgroup_setting.h"
#include "geodatapointgroup_updateactorsettingcommand.h"
#include "ui_geodatapointgroup_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <QFileInfo>
#include <QFileDialog>
#include <QImage>

GeoDataPointGroup::PropertyDialog::PropertyDialog(GeoDataPointGroup* group, QWidget *parent) :
	QDialog (parent),
	m_group {group},
	ui(new Ui::GeoDataPointGroup_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect(ui->importImageButton, &QPushButton::clicked, [=](bool) {importImage();});
	connect(ui->clearImageButton, &QPushButton::clicked, [=](bool) {clearImage();});
	connect<void(QSpinBox::*)(int)>(ui->maxSizeSpinBox, &QSpinBox::valueChanged, [=](int) {updateImage();});

	if (m_group->gridAttribute()->isReferenceInformation()) {
		ui->arbitraryRadioButton->setDisabled(true);
		ui->byValueRadioButton->setDisabled(true);

		ui->arbitraryRadioButton->setChecked(true);
	}

	setSetting(m_group->impl->m_setting);
}

GeoDataPointGroup::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* GeoDataPointGroup::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPointGroup::PropertyDialog::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<Setting>(iRIC::generateCommandId("GeoDataPointGroup::PropertyDialog::Basic"), true, setting(), &m_group->impl->m_setting));
	return new UpdateActorSettingCommand(command, m_group);
}

void GeoDataPointGroup::PropertyDialog::accept()
{
	auto item = m_group->geoDataDataItem();
	item->pushRenderCommand(createModifyCommand(false), item);

	QDialog::accept();
}

void GeoDataPointGroup::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void GeoDataPointGroup::PropertyDialog::importImage()
{
	auto fname = QFileDialog::getOpenFileName(this, tr("Select image file"), LastIODirectory::get(),
																						tr("All images(*.jpg *.jpeg *.png);;Jpeg images(*.jpg *.jpeg);;PNG images(*.png)"));
	if (fname.isNull()) {return;}

	QImage image(fname);
	m_pixmap = QPixmap::fromImage(image);

	updateImage();
}

void GeoDataPointGroup::PropertyDialog::updateImage()
{
	if (m_pixmap.isNull()) {
		ui->imageLabel->setPixmap(QPixmap());
	}

	ui->imageLabel->setPixmap(Impl::shrinkPixmap(m_pixmap, ui->maxSizeSpinBox->value()));
}

void GeoDataPointGroup::PropertyDialog::clearImage()
{
	m_pixmap = QPixmap();
	updateImage();
}

void GeoDataPointGroup::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

GeoDataPointGroup::Setting GeoDataPointGroup::PropertyDialog::setting() const
{
	Setting setting;

	if (ui->arbitraryRadioButton->isChecked()) {
		setting.mapping = Setting::Mapping::Arbitrary;
	} else if (ui->byValueRadioButton->isChecked()) {
		setting.mapping = Setting::Mapping::Value;
	}
	if (ui->pointsRadioButton->isChecked()) {
		setting.shape = Setting::Shape::Point;
	} else if (ui->imagesRadioButton->isChecked()) {
		setting.shape = Setting::Shape::Image;
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

void GeoDataPointGroup::PropertyDialog::setSetting(const Setting& setting)
{
	if (setting.mapping == Setting::Mapping::Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	} else if (setting.mapping == Setting::Mapping::Value) {
		ui->byValueRadioButton->setChecked(true);
	}
	if (setting.shape == Setting::Shape::Point) {
		ui->pointsRadioButton->setChecked(true);
	} else if (setting.shape == Setting::Shape::Image) {
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

void GeoDataPointGroup::PropertyDialog::apply()
{
	auto item = m_group->geoDataDataItem();
	item->pushRenderCommand(createModifyCommand(true), item);
}
