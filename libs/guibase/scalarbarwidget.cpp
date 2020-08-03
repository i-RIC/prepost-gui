#include "ui_scalarbarwidget.h"

#include "scalarbarsetting.h"
#include "scalarbarwidget.h"
#include "vtktextpropertysettingdialog.h"

namespace {

void editVtkTextPropertySetting(vtkTextPropertySettingContainer* c, QWidget* parent)
{
	vtkTextPropertySettingDialog dialog(parent);
	dialog.setSetting(*c);

	dialog.hideSize();

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	*c = dialog.setting();
}

} // namespace

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

ScalarBarSetting ScalarBarWidget::setting() const
{
	ScalarBarSetting ret = m_setting;

	ret.visible = ui->displayCheckBox->isChecked();
	if (ui->horizontalRadioButton->isChecked()) {
		ret.orientation = ScalarBarSetting::Orientation::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		ret.orientation = ScalarBarSetting::Orientation::Vertical;
	}
	ret.numberOfLabels = ui->numberSpinBox->value();
	ret.width = ui->widthSpinBox->value();
	ret.height = ui->heightSpinBox->value();
	ret.positionX = ui->positionXSpinBox->value();
	ret.positionY = ui->positionYSpinBox->value();
	ret.labelFormat = ui->labelFormatEdit->text();

	return ret;
}

void ScalarBarWidget::setSetting(const ScalarBarSetting& setting)
{
	m_setting = setting;

	ui->displayCheckBox->setChecked(setting.visible);
	if (setting.orientation == ScalarBarSetting::Orientation::Horizontal) {
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

QString ScalarBarWidget::title() const
{
	return ui->titleEdit->text().trimmed();
}

void ScalarBarWidget::setTitle(const QString& title)
{
	ui->titleEdit->setText(title.trimmed());
}

void ScalarBarWidget::editTitleTextSetting()
{
	editVtkTextPropertySetting(&(m_setting.titleTextSetting), this);
}

void ScalarBarWidget::editLabelTextSetting()
{
	editVtkTextPropertySetting(&(m_setting.labelTextSetting), this);
}
