#include "iricmainwindow_discardresultconfirmdialog.h"
#include "ui_iricmainwindow_discardresultconfirmdialog.h"

iRICMainWindow::DiscardResultConfirmDialog::DiscardResultConfirmDialog(QWidget *parent) :
	QDialog(parent),
	m_result {Result::DiscardGrid},
	ui(new Ui::iRICMainWindow_DiscardResultConfirmDialog)
{
	ui->setupUi(this);

	connect(ui->discardGridButton, &QPushButton::clicked, this, &DiscardResultConfirmDialog::submitDiscardGrid);
	connect(ui->discardResultButton, &QPushButton::clicked, this, &DiscardResultConfirmDialog::submitDiscardResult);
	connect(ui->cancelButton, &QPushButton::clicked, this, &DiscardResultConfirmDialog::reject);
}

iRICMainWindow::DiscardResultConfirmDialog::~DiscardResultConfirmDialog()
{
	delete ui;
}

iRICMainWindow::DiscardResultConfirmDialog::Result iRICMainWindow::DiscardResultConfirmDialog::result() const
{
	return m_result;
}

void iRICMainWindow::DiscardResultConfirmDialog::submitDiscardGrid()
{
	m_result = Result::DiscardGrid;
	QDialog::accept();
}

void iRICMainWindow::DiscardResultConfirmDialog::submitDiscardResult()
{
	m_result = Result::DiscardCalculationResult;
	QDialog::accept();
}
