#include "ui_graph2dhybridwindowlinesettingdialog.h"

#include "graph2dhybridwindowlinesettingdialog.h"

Graph2dHybridWindowLineSettingDialog::Graph2dHybridWindowLineSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dHybridWindowLineSettingDialog)
{
	ui->setupUi(this);
}

Graph2dHybridWindowLineSettingDialog::~Graph2dHybridWindowLineSettingDialog()
{
	delete ui;
}

void Graph2dHybridWindowLineSettingDialog::setLineWidth(int width)
{
	ui->widthSpinBox->setValue(width);
}

int Graph2dHybridWindowLineSettingDialog::lineWidth()
{
	return ui->widthSpinBox->value();
}

void Graph2dHybridWindowLineSettingDialog::setCustomColor(QColor col)
{
	ui->colorWidget->setColor(col);
}

QColor Graph2dHybridWindowLineSettingDialog::customColor()
{
	return ui->colorWidget->color();
}

void Graph2dHybridWindowLineSettingDialog::setAxisSide(Graph2dHybridWindowResultSetting::AxisSide side)
{
	if (side == Graph2dHybridWindowResultSetting::asLeft) {
		ui->yAxisLeftRadioButton->setChecked(true);
	} else {
		ui->yAxisRightRadioButton->setChecked(true);
	}

}

Graph2dHybridWindowResultSetting::AxisSide Graph2dHybridWindowLineSettingDialog::axisSide()
{
	if (ui->yAxisLeftRadioButton->isChecked()) {
		return Graph2dHybridWindowResultSetting::asLeft;
	} else {
		return Graph2dHybridWindowResultSetting::asRight;
	}
}
