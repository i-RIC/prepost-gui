#include "ui_geodatabackgroundeditdialog.h"

#include "../gridcond/base/gridattributeeditwidget.h"
#include "geodatabackgroundeditdialog.h"

GeoDataBackgroundEditDialog::GeoDataBackgroundEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataBackgroundEditDialog)
{
	ui->setupUi(this);
	connect(ui->minRadioButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetStatus()));
	connect(ui->maxRadioButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetStatus()));
	connect(ui->customRadioButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetStatus()));
}

GeoDataBackgroundEditDialog::~GeoDataBackgroundEditDialog()
{
	delete ui;
}

void GeoDataBackgroundEditDialog::setWidget(GridAttributeEditWidget* w)
{
	ui->customWidget->setWidget(w);
	adjustSize();
}

void GeoDataBackgroundEditDialog::setupDialog()
{
	if (m_type == GeoDataBackground::Custom) {
		ui->customRadioButton->click();
		ui->customWidget->widget()->setVariantValue(m_customValue);
	} else if (m_type == GeoDataBackground::Maximum) {
		ui->maxRadioButton->click();
	} else {
		ui->minRadioButton->click();
	}
}

void GeoDataBackgroundEditDialog::accept()
{
	if (ui->customRadioButton->isChecked()) {
		m_type = GeoDataBackground::Custom;
		m_customValue = ui->customWidget->widget()->variantValue();
	} else if (ui->maxRadioButton->isChecked()) {
		m_type = GeoDataBackground::Maximum;
	} else {
		m_type = GeoDataBackground::Minimum;
	}
	QDialog::accept();
}

void GeoDataBackgroundEditDialog::updateCustomWidgetStatus()
{
	if (ui->customRadioButton->isChecked()) {
		ui->customWidget->setEnabled(true);
	} else {
		ui->customWidget->setDisabled(true);
	}
}
