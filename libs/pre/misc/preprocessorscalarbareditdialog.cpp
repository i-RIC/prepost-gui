#include "preprocessorscalarbareditdialog.h"
#include "ui_preprocessorscalarbareditdialog.h"

PreProcessorScalarBarEditDialog::PreProcessorScalarBarEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorScalarBarEditDialog)
{
	ui->setupUi(this);
	ui->scalarBarWidget->setDisplayScalarBar(true);
}

PreProcessorScalarBarEditDialog::~PreProcessorScalarBarEditDialog()
{
	delete ui;
}

void PreProcessorScalarBarEditDialog::setScalarBarTitle(const QString& title)
{
	ui->scalarBarWidget->setTitle(title);
}

void PreProcessorScalarBarEditDialog::setOrientation(ScalarBarSetting::Orientation o)
{
	ui->scalarBarWidget->setOrientation(o);
}

void PreProcessorScalarBarEditDialog::setNumberOfLabels(int num)
{
	ui->scalarBarWidget->setNumberOfLabels(num);
}

void PreProcessorScalarBarEditDialog::setWidth(double width)
{
	ui->scalarBarWidget->setWidth(width);
}

void PreProcessorScalarBarEditDialog::setHeight(double height)
{
	ui->scalarBarWidget->setHeight(height);
}

void PreProcessorScalarBarEditDialog::setPositionX(double x)
{
	ui->scalarBarWidget->setPositionX(x);
}

void PreProcessorScalarBarEditDialog::setPositionY(double y)
{
	ui->scalarBarWidget->setPositionY(y);
}

void PreProcessorScalarBarEditDialog::setColorBarSetting(PreProcessorWindowScalarBarSettingWidget::Setting s)
{
	ui->scalarBarWidget->setSetting(s);
}

QString PreProcessorScalarBarEditDialog::scalarBarTitle() const
{
	return ui->scalarBarWidget->title();
}

ScalarBarSetting::Orientation PreProcessorScalarBarEditDialog::orientation() const
{
	return ui->scalarBarWidget->orientation();
}

int PreProcessorScalarBarEditDialog::numberOfLabels() const
{
	return ui->scalarBarWidget->numberOfLabels();
}

double PreProcessorScalarBarEditDialog::width() const
{
	return ui->scalarBarWidget->width();
}

double PreProcessorScalarBarEditDialog::height() const
{
	return ui->scalarBarWidget->height();
}

double PreProcessorScalarBarEditDialog::positionX() const
{
	return ui->scalarBarWidget->positionX();
}

double PreProcessorScalarBarEditDialog::positionY() const
{
	return ui->scalarBarWidget->positionY();
}

PreProcessorWindowScalarBarSettingWidget::Setting PreProcessorScalarBarEditDialog::colorBarSetting() const
{
	return ui->scalarBarWidget->setting();
}

void PreProcessorScalarBarEditDialog::hideDisplayCheckBox()
{
	ui->scalarBarWidget->hideDisplayCheckBox();
}


void PreProcessorScalarBarEditDialog::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	ui->scalarBarWidget->setTitleTextSetting(cont);
}

void PreProcessorScalarBarEditDialog::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	ui->scalarBarWidget->setLabelTextSetting(cont);
}

void PreProcessorScalarBarEditDialog::setLabelFormat(const QString& format)
{
	ui->scalarBarWidget->setLabelFormat(format);
}

const vtkTextPropertySettingContainer& PreProcessorScalarBarEditDialog::titleTextSetting() const
{
	return ui->scalarBarWidget->titleTextSetting();
}

const vtkTextPropertySettingContainer& PreProcessorScalarBarEditDialog::labelTextSetting() const
{
	return ui->scalarBarWidget->labelTextSetting();
}

const QString PreProcessorScalarBarEditDialog::labelFormat() const
{
	return ui->scalarBarWidget->labelFormat();
}
