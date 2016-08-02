#include "vtklinestylecontainer.h"
#include "vtklinestylewidget.h"
#include "ui_vtklinestylewidget.h"

vtkLineStyleWidget::vtkLineStyleWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::vtkLineStyleWidget)
{
	ui->setupUi(this);
}

vtkLineStyleWidget::~vtkLineStyleWidget()
{
	delete ui;
}

vtkLineStyleContainer vtkLineStyleWidget::lineStyle() const
{
	vtkLineStyleContainer ret;
	ret.color = ui->colorWidget->color();
	ret.width = ui->widthSpinBox->value();

	switch (ui->styleComboBox->currentIndex()) {
	case 0:
		ret.linePattern = vtkLineStyleContainer::Solid;
		break;
	case 1:
		ret.linePattern = vtkLineStyleContainer::Dashed;
		break;
	case 2:
		ret.linePattern = vtkLineStyleContainer::Dotted;
		break;
	}

	return ret;
}

void vtkLineStyleWidget::setLineStyle(const vtkLineStyleContainer& c)
{
	ui->colorWidget->setColor(c.color);
	ui->widthSpinBox->setValue(c.width);
	switch (c.linePattern) {
	case vtkLineStyleContainer::Solid:
		ui->styleComboBox->setCurrentIndex(0);
		break;
	case vtkLineStyleContainer::Dashed:
		ui->styleComboBox->setCurrentIndex(1);
		break;
	case vtkLineStyleContainer::Dotted:
		ui->styleComboBox->setCurrentIndex(2);
		break;
	}
}
