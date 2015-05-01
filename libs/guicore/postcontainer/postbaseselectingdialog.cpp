#include "postbaseselectingdialog.h"
#include "ui_postbaseselectingdialog.h"

PostBaseSelectingDialog::PostBaseSelectingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PostBaseSelectingDialog)
{
	ui->setupUi(this);
}

PostBaseSelectingDialog::~PostBaseSelectingDialog()
{
	delete ui;
}

void PostBaseSelectingDialog::setAvailableDimensions(int dimensions)
{
	if ((dimensions & Dimension1) == Dimension1) {
		ui->dim1radioButton->setEnabled(true);
		ui->dim1radioButton->setChecked(true);
		ui->dim1radioButton->setFocus();
	} else {
		ui->dim1radioButton->setEnabled(false);
	}
	if ((dimensions & Dimension2) == Dimension2) {
		ui->dim2radioButton->setEnabled(true);
		ui->dim2radioButton->setChecked(true);
		ui->dim2radioButton->setFocus();
	} else {
		ui->dim2radioButton->setEnabled(false);
	}
	if ((dimensions & Dimension3) == Dimension3) {
		ui->dim3radioButton->setEnabled(true);
		ui->dim3radioButton->setChecked(true);
		ui->dim3radioButton->setFocus();
	} else {
		ui->dim3radioButton->setEnabled(false);
	}
}

PostBaseSelectingDialog::BaseDimension PostBaseSelectingDialog::selectedDimension()
{
	if (ui->dim1radioButton->isChecked()) {
		return Dimension1;
	}
	if (ui->dim2radioButton->isChecked()) {
		return Dimension2;
	}
	if (ui->dim3radioButton->isChecked()) {
		return Dimension3;
	}
	// default is 3.
	return Dimension3;
}

PostSolutionInfo::Dimension PostBaseSelectingDialog::selectedPostDimension()
{
	BaseDimension dim = selectedDimension();
	return getPostDimension(dim);
}

PostSolutionInfo::Dimension PostBaseSelectingDialog::getPostDimension(BaseDimension dim)
{
	if (dim == Dimension1) {return PostSolutionInfo::dim1D;}
	if (dim == Dimension2) {return PostSolutionInfo::dim2D;}
	if (dim == Dimension3) {return PostSolutionInfo::dim3D;}
	return PostSolutionInfo::dim3D;
}

