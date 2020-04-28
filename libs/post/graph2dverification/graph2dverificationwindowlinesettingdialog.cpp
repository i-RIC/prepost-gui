#include "ui_graph2dverificationwindowlinesettingdialog.h"

#include "graph2dverificationwindowlinesettingdialog.h"

Graph2dVerificationWindowLineSettingDialog::Graph2dVerificationWindowLineSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dVerificationWindowLineSettingDialog)
{
	ui->setupUi(this);
}

Graph2dVerificationWindowLineSettingDialog::~Graph2dVerificationWindowLineSettingDialog()
{
	delete ui;
}

int Graph2dVerificationWindowLineSettingDialog::lineWidth() const
{
	return ui->widthSpinBox->value();
}

void Graph2dVerificationWindowLineSettingDialog::setLineWidth(int width)
{
	ui->widthSpinBox->setValue(width);
}

QColor Graph2dVerificationWindowLineSettingDialog::customColor() const
{
	return ui->colorWidget->color();
}

void Graph2dVerificationWindowLineSettingDialog::setCustomColor(QColor col)
{
	ui->colorWidget->setColor(col);
}
