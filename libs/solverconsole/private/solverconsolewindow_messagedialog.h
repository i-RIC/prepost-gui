#ifndef SOLVERCONSOLEWINDOW_MESSAGEDIALOG_H
#define SOLVERCONSOLEWINDOW_MESSAGEDIALOG_H

#include "../solverconsolewindow.h"

#include <QDialog>

namespace Ui {
class SolverConsoleWindow_MessageDialog;
}

class SolverConsoleWindow::MessageDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MessageDialog(QWidget *parent = nullptr);
	~MessageDialog();

private:
	Ui::SolverConsoleWindow_MessageDialog *ui;
};

#endif // SOLVERCONSOLEWINDOW_MESSAGEDIALOG_H
