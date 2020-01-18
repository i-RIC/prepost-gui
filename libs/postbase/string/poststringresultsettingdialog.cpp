#include "poststringresultsettingdialog.h"
#include "ui_poststringresultsettingdialog.h"

#include <QFontDialog>

PostStringResultSettingDialog::PostStringResultSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostStringResultSettingDialog)
{
	ui->setupUi(this);
	connect(ui->fontEditButton, SIGNAL(clicked()), this, SLOT(editFont()));
}

PostStringResultSettingDialog::~PostStringResultSettingDialog()
{
	delete ui;
}

void PostStringResultSettingDialog::setZoneDataContainer(PostZoneDataContainer* container)
{
	ui->settingWidget->setZoneDataContainer(container);
}

PostStringResult* PostStringResultSettingDialog::stringResult() const
{
	return ui->settingWidget->stringResult();
}

void PostStringResultSettingDialog::setStringResult(PostStringResult* result)
{
	ui->settingWidget->setStringResult(result);
}

PostStringResultDataItem::Setting PostStringResultSettingDialog::displaySetting() const
{
	PostStringResultDataItem::Setting ret;

	ret.autoSize = ui->autoCheckBox->isChecked();
	ret.hMargin = ui->hMarginSpinBox->value();
	ret.vMargin = ui->vMarginSpinBox->value();
	ret.left = ui->leftSpinBox->value();
	ret.top = ui->topSpinBox->value();
	ret.width = ui->widthSpinBox->value();
	ret.height = ui->heightSpinBox->value();
	ret.fontColor = ui->fontColorEditWidget->color();
	ret.font = m_font;
	ret.backgroundColor = ui->backgroundColorEditWidget->color();

	return ret;
}

void PostStringResultSettingDialog::setDisplaySetting(const PostStringResultDataItem::Setting& setting)
{
	ui->autoCheckBox->setChecked(setting.autoSize);
	ui->hMarginSpinBox->setValue(setting.hMargin);
	ui->vMarginSpinBox->setValue(setting.vMargin);
	ui->leftSpinBox->setValue(setting.left);
	ui->topSpinBox->setValue(setting.top);
	ui->widthSpinBox->setValue(setting.width);
	ui->heightSpinBox->setValue(setting.height);
	ui->fontColorEditWidget->setColor(setting.fontColor);
	ui->backgroundColorEditWidget->setColor(setting.backgroundColor);
	m_font = setting.font;
	updateFontLabel();
}

void PostStringResultSettingDialog::editFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_font, this);
	if (! ok) {return;}

	m_font = newFont;
	updateFontLabel();
}

void PostStringResultSettingDialog::accept()
{
	bool ok = ui->settingWidget->check();
	if (! ok) {return;}

	QDialog::accept();
}

void PostStringResultSettingDialog::updateFontLabel()
{
	QString fontStr = m_font.family();
	if (m_font.pixelSize() != -1) {
		fontStr += QString(" %1 px").arg(m_font.pixelSize());
	} else if (m_font.pointSize() != -1) {
		fontStr += QString(" %1 pt").arg(m_font.pointSize());
	}
	ui->fontDisplayLabel->setText(fontStr);
}
