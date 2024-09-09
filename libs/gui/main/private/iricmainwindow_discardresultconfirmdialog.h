#ifndef IRICMAINWINDOW_DISCARDRESULTCONFIRMDIALOG_H
#define IRICMAINWINDOW_DISCARDRESULTCONFIRMDIALOG_H

#include "../iricmainwindow.h"

#include <QDialog>

namespace Ui {
class iRICMainWindow_DiscardResultConfirmDialog;
}

class iRICMainWindow::DiscardResultConfirmDialog : public QDialog
{
	Q_OBJECT

public:
	enum class Result {
		DiscardGrid,
		DiscardCalculationResult
	};

	explicit DiscardResultConfirmDialog(QWidget *parent = nullptr);
	~DiscardResultConfirmDialog();

	Result result() const;

private slots:
	void submitDiscardGrid();
	void submitDiscardResult();

private:
	Result m_result;

	Ui::iRICMainWindow_DiscardResultConfirmDialog *ui;
};

#endif // IRICMAINWINDOW_DISCARDRESULTCONFIRMDIALOG_H
