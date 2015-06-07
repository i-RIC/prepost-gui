#include "ui_scalarbarwidget.h"

#include "scalarbarsetting.h"
#include "scalarbarwidget.h"
#include "vtktextpropertysettingdialog.h"

ScalarBarWidget::ScalarBarWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ScalarBarWidget)
{
	ui->setupUi(this);
	connect(ui->titleFontButton, SIGNAL(clicked()), this, SLOT(editTitleTextSetting()));
	connect(ui->labelFontButton, SIGNAL(clicked()), this, SLOT(editLabelTextSetting()));
}

ScalarBarWidget::~ScalarBarWidget()
{
	delete ui;
}

void ScalarBarWidget::setSetting(const ScalarBarSetting& setting)
{
	ui->displayCheckBox->setChecked(setting.visible);
	if (setting.orientation == ScalarBarSetting::oHorizontal) {
		ui->horizontalRadioButton->setChecked(true);
	} else {
		ui->verticalRadioButton->setChecked(true);
	}
	ui->numberSpinBox->setValue(setting.numberOfLabels);
	ui->widthSpinBox->setValue(setting.width);
	ui->heightSpinBox->setValue(setting.height);
	ui->positionXSpinBox->setValue(setting.positionX);
	ui->positionYSpinBox->setValue(setting.positionY);
	ui->labelFormatEdit->setText(setting.labelFormat);
}

void ScalarBarWidget::setTitle(const QString& title)
{
	ui->titleEdit->setText(title.trimmed());
}

const ScalarBarSetting ScalarBarWidget::setting() const
{
	ScalarBarSetting ret;
	ret.visible = ui->displayCheckBox->isChecked();
	if (ui->horizontalRadioButton->isChecked()) {
		ret.orientation = ScalarBarSetting::oHorizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		ret.orientation = ScalarBarSetting::oVertical;
	}
	ret.numberOfLabels = ui->numberSpinBox->value();
	ret.width = ui->widthSpinBox->value();
	ret.height = ui->heightSpinBox->value();
	ret.positionX = ui->positionXSpinBox->value();
	ret.positionY = ui->positionYSpinBox->value();
	ret.labelFormat = ui->labelFormatEdit->text();
	return ret;
}

QString ScalarBarWidget::title() const
{
	return ui->titleEdit->text().trimmed();
}

void ScalarBarWidget::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_titleTextSetting = cont;
}

void ScalarBarWidget::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_labelTextSetting = cont;
}

void ScalarBarWidget::editTitleTextSetting()
{
	vtkTextPropertySettingDialog dialog(this);
	dialog.setSetting(m_titleTextSetting);
	dialog.disableSize();
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_titleTextSetting = dialog.setting();
}

void ScalarBarWidget::editLabelTextSetting()
{
	vtkTextPropertySettingDialog dialog(this);
	dialog.setSetting(m_labelTextSetting);
	dialog.disableSize();
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_labelTextSetting = dialog.setting();
}
