#include "ui_waitdialog.h"

#include "waitdialog.h"

#include <cmath>

WaitDialog::WaitDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::WaitDialog)
{
	setModal(true);
	ui->setupUi(this);
	ui->progressBar->hide();
	m_unknownLimitMode = false;
}

WaitDialog::~WaitDialog()
{
	delete ui;
}

void WaitDialog::setMessage(const QString& message)
{
	ui->messageLabel->setText(message);
}

void WaitDialog::showProgressBar()
{
	ui->progressBar->show();
}

void WaitDialog::disableCancelButton()
{
	ui->buttonBox->setDisabled(true);
}

void WaitDialog::setRange(int min, int max)
{
	ui->progressBar->setRange(min, max);
}

void WaitDialog::setProgress(int value)
{
	if (m_unknownLimitMode) {
		int val = 100 * (1 - exp(- value / static_cast<double>(m_param)));
		ui->progressBar->setValue(val);
	} else {
		ui->progressBar->setValue(value);
	}
}

int WaitDialog::progress() const
{
	if (m_unknownLimitMode) {
		// does not work correctly.
		return 0;
	}
	return ui->progressBar->value();
}

void WaitDialog::setFinished()
{
	ui->progressBar->setValue(ui->progressBar->maximum());
}

void WaitDialog::reject()
{
	emit canceled();
}

void WaitDialog::setUnknownLimitMode(int param)
{
	m_unknownLimitMode = true;
	m_param = param;
}
