#include "ui_gridshapeeditdialog.h"

#include "gridshapeeditdialog.h"

GridShapeEditDialog::GridShapeEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridShapeEditDialog)
{
	ui->setupUi(this);

	connect(ui->outlineRadioButton, SIGNAL(toggled(bool)), this, SLOT(outlineToggled(bool)));
	setMaximumSize(sizeHint());
}

GridShapeEditDialog::~GridShapeEditDialog()
{
	delete ui;
}

void GridShapeEditDialog::hideShape()
{
	ui->allRadioButton->setChecked(true);
	ui->allRadioButton->hide();
	ui->outlineRadioButton->hide();
	adjustSize();
	setMaximumSize(sizeHint());
}

void GridShapeEditDialog::hideIndex()
{
	ui->indicesGroupBox->hide();
	adjustSize();
	setMaximumSize(sizeHint());
}

void GridShapeEditDialog::setGridColor(const QColor& color)
{
	ui->gridColorEditWidget->setColor(color);
}

QColor GridShapeEditDialog::gridColor()
{
	return 	ui->gridColorEditWidget->color();
}

void GridShapeEditDialog::setIndexVisible(bool visible)
{
	ui->indexVisibleCheckBox->setChecked(visible);
}

bool GridShapeEditDialog::indexVisible()
{
	return ui->indexVisibleCheckBox->isChecked();
}

void GridShapeEditDialog::setIndexColor(const QColor& color)
{
	ui->indexColorEditWidget->setColor(color);
}

QColor GridShapeEditDialog::indexColor()
{
	return ui->indexColorEditWidget->color();
}

void GridShapeEditDialog::setShape(Shape shape)
{
	if (shape == Outline) {
		ui->outlineRadioButton->setChecked(true);
	} else {
		ui->allRadioButton->setChecked(true);
	}
}

GridShapeEditDialog::Shape GridShapeEditDialog::shape()
{
	if (ui->outlineRadioButton->isChecked()) {
		return Outline;
	} else {
		return Wireframe;
	}
}

void GridShapeEditDialog::outlineToggled(bool toggled)
{
	if (toggled) {
		ui->indexVisibleCheckBox->setChecked(false);
	}
}
