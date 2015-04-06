#include "preprocessorwindowscalarbarsettingwidget.h"
#include "ui_preprocessorwindowscalarbarsettingwidget.h"
#include <guibase/vtktextpropertysettingdialog.h>

PreProcessorWindowScalarBarSettingWidget::Setting::Setting()
{
	visible = true;
	orientation = ScalarBarSetting::oVertical;
	numberOfLabels = DEFAULT_NUMOFLABELS;
	width = 0.1;
	height = 0.3;
	positionX = 0.8;
	positionY = 0.1;
}

PreProcessorWindowScalarBarSettingWidget::PreProcessorWindowScalarBarSettingWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PreProcessorWindowScalarBarSettingWidget)
{
	ui->setupUi(this);

	m_orientationGroup = new QButtonGroup(this);
	m_orientationGroup->addButton(ui->verticalRadioButton);
	m_orientationGroup->addButton(ui->horizontalRadioButton);
	m_titleGroup = new QButtonGroup(this);

	// build connections
	connect(ui->displayCheckBox, SIGNAL(clicked(bool)), this, SLOT(setDisplayScalarBar(bool)));
	connect(ui->titleFontButton, SIGNAL(clicked()), this, SLOT(editTitleTextSetting()));
	connect(ui->labelFontButton, SIGNAL(clicked()), this, SLOT(editLabelTextSetting()));
}

PreProcessorWindowScalarBarSettingWidget::~PreProcessorWindowScalarBarSettingWidget()
{
	delete ui;
}

void PreProcessorWindowScalarBarSettingWidget::setVisibility(bool visible)
{
	ui->displayCheckBox->setChecked(false);
	ui->displayCheckBox->click();
	if(! visible){
		ui->displayCheckBox->click();
	}
}

void PreProcessorWindowScalarBarSettingWidget::setOrientation(ScalarBarSetting::Orientation o)
{
	switch(o){
	case ScalarBarSetting::oVertical:
		ui->verticalRadioButton->click();
		break;
	case ScalarBarSetting::oHorizontal:
		ui->horizontalRadioButton->click();
		break;
	}
}

void PreProcessorWindowScalarBarSettingWidget::setTitle(const QString& title)
{
	ui->titleEdit->setText(title);
}

void PreProcessorWindowScalarBarSettingWidget::setNumberOfLabels(int labels)
{
	ui->numberSpinBox->setValue(labels);
}

void PreProcessorWindowScalarBarSettingWidget::setMaxNumberOfLabels(int max)
{
	ui->numberSpinBox->setMaximum(max);
}

void PreProcessorWindowScalarBarSettingWidget::setWidth(double width)
{
	ui->widthSpinBox->setValue(width);
}

void PreProcessorWindowScalarBarSettingWidget::setHeight(double height)
{
	ui->heightSpinBox->setValue(height);
}

void PreProcessorWindowScalarBarSettingWidget::setPositionX(double x)
{
	ui->positionXSpinBox->setValue(x);
}

void PreProcessorWindowScalarBarSettingWidget::setPositionY(double y)
{
	ui->positionYSpinBox->setValue(y);
}

void PreProcessorWindowScalarBarSettingWidget::setSetting(const Setting& setting)
{
	setVisibility(setting.visible);
	setNumberOfLabels(setting.numberOfLabels);
	setOrientation(setting.orientation);
	setPositionX(setting.positionX);
	setPositionY(setting.positionY);
	setWidth(setting.width);
	setHeight(setting.height);
}

void PreProcessorWindowScalarBarSettingWidget::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_titleTextSetting = cont;
}

void PreProcessorWindowScalarBarSettingWidget::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_labelTextSetting = cont;
}

bool PreProcessorWindowScalarBarSettingWidget::visibility() const
{
	return ui->displayCheckBox->isChecked();
}

ScalarBarSetting::Orientation PreProcessorWindowScalarBarSettingWidget::orientation() const
{
	if (ui->verticalRadioButton->isChecked()) return ScalarBarSetting::oVertical;
	if (ui->horizontalRadioButton->isChecked()) return ScalarBarSetting::oHorizontal;
	// default
	return ScalarBarSetting::oVertical;
}

QString PreProcessorWindowScalarBarSettingWidget::title() const
{
	return ui->titleEdit->text();
}

int PreProcessorWindowScalarBarSettingWidget::numberOfLabels() const
{
	return ui->numberSpinBox->value();
}

double PreProcessorWindowScalarBarSettingWidget::width() const
{
	return ui->widthSpinBox->value();
}

double PreProcessorWindowScalarBarSettingWidget::height() const
{
	return ui->heightSpinBox->value();
}

double PreProcessorWindowScalarBarSettingWidget::positionX() const
{
	return ui->positionXSpinBox->value();
}

double PreProcessorWindowScalarBarSettingWidget::positionY() const
{
	return ui->positionYSpinBox->value();
}

void PreProcessorWindowScalarBarSettingWidget::setTitleModeToManual(bool checked)
{
	ui->titleEdit->setEnabled(checked);
}

void PreProcessorWindowScalarBarSettingWidget::setDisplayScalarBar(bool checked)
{
	ui->titleEdit->setEnabled(checked);
	ui->verticalRadioButton->setEnabled(checked);
	ui->horizontalRadioButton->setEnabled(checked);
	ui->numberSpinBox->setEnabled(checked);
	ui->widthSpinBox->setEnabled(checked);
	ui->heightSpinBox->setEnabled(checked);
	ui->positionXSpinBox->setEnabled(checked);
	ui->positionYSpinBox->setEnabled(checked);
}

void PreProcessorWindowScalarBarSettingWidget::hideDisplayCheckBox()
{
	ui->displayCheckBox->hide();
}

PreProcessorWindowScalarBarSettingWidget::Setting PreProcessorWindowScalarBarSettingWidget::setting() const
{
	Setting s;
	s.visible = visibility();
	s.numberOfLabels = numberOfLabels();
	s.orientation = orientation();
	s.positionX = positionX();
	s.positionY = positionY();
	s.width = width();
	s.height = height();

	return s;
}

void PreProcessorWindowScalarBarSettingWidget::editTitleTextSetting()
{
	vtkTextPropertySettingDialog dialog(this);
	dialog.setSetting(m_titleTextSetting);
	dialog.disableSize();
	int ret = dialog.exec();
	if (ret == QDialog::Rejected){return;}

	m_titleTextSetting = dialog.setting();
}

void PreProcessorWindowScalarBarSettingWidget::editLabelTextSetting()
{
	vtkTextPropertySettingDialog dialog(this);
	dialog.setSetting(m_labelTextSetting);
	dialog.disableSize();
	int ret = dialog.exec();
	if (ret == QDialog::Rejected){return;}

	m_labelTextSetting = dialog.setting();
}

void PreProcessorWindowScalarBarSettingWidget::setLabelFormat(const QString& format)
{
	ui->labelFormatEdit->setText(format);
}

const QString PreProcessorWindowScalarBarSettingWidget::labelFormat()
{
	return ui->labelFormatEdit->text();
}
