#include "ui_rawdatabackgroundeditdialog.h"

#include "../gridcond/base/gridattributeeditwidget.h"
#include "rawdatabackgroundeditdialog.h"

RawDataBackgroundEditDialog::RawDataBackgroundEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::RawDataBackgroundEditDialog)
{
	ui->setupUi(this);
	connect(ui->minRadioButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetStatus()));
	connect(ui->maxRadioButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetStatus()));
	connect(ui->customRadioButton, SIGNAL(clicked()), this, SLOT(updateCustomWidgetStatus()));
}

RawDataBackgroundEditDialog::~RawDataBackgroundEditDialog()
{
	delete ui;
}

void RawDataBackgroundEditDialog::setWidget(GridAttributeEditWidget* w)
{
	ui->customWidget->setWidget(w);
	adjustSize();
}

void RawDataBackgroundEditDialog::setupDialog()
{
	if (m_type == RawDataBackground::Custom) {
		ui->customRadioButton->click();
		ui->customWidget->widget()->setVariantValue(m_customValue);
	} else if (m_type == RawDataBackground::Maximum) {
		ui->maxRadioButton->click();
	} else {
		ui->minRadioButton->click();
	}
}

void RawDataBackgroundEditDialog::accept()
{
	if (ui->customRadioButton->isChecked()) {
		m_type = RawDataBackground::Custom;
		m_customValue = ui->customWidget->widget()->variantValue();
	} else if (ui->maxRadioButton->isChecked()) {
		m_type = RawDataBackground::Maximum;
	} else {
		m_type = RawDataBackground::Minimum;
	}
	QDialog::accept();
}

void RawDataBackgroundEditDialog::updateCustomWidgetStatus()
{
	if (ui->customRadioButton->isChecked()) {
		ui->customWidget->setEnabled(true);
	} else {
		ui->customWidget->setDisabled(true);
	}
}
