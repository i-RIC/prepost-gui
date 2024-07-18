#include "solverconsolewindow_messagedialog.h"
#include "ui_solverconsolewindow_messagedialog.h"

SolverConsoleWindow::MessageDialog::MessageDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SolverConsoleWindow_MessageDialog)
{
	ui->setupUi(this);
}

SolverConsoleWindow::MessageDialog::~MessageDialog()
{
	delete ui;
}
