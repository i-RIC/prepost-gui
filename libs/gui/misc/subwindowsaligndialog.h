#ifndef SUBWINDOWSALIGNDIALOG_H
#define SUBWINDOWSALIGNDIALOG_H

#include <QDialog>

namespace Ui {
class SubwindowsAlignDialog;
}

class QMdiArea;

class SubwindowsAlignDialog : public QDialog
{
	Q_OBJECT

public:
	SubwindowsAlignDialog(QMdiArea* area, QWidget *parent = nullptr);
	~SubwindowsAlignDialog();

public slots:
	void accept() override;

private slots:
	void updateMarginWidgetStatus();

private:
	void alignSubWindows();
	void alignSubWindowsHorizontal(const std::vector<QMdiSubWindow*>& subWindows);
	void alignSubWindowsVertical(const std::vector<QMdiSubWindow*>& subWindows);
	void alignSubWindowsGrid(const std::vector<QMdiSubWindow*>& subWindows);
	void alignSubWindowsGridForColumns(const std::vector<QMdiSubWindow*>& subWindows);
	void alignSubWindowsGridForRows(const std::vector<QMdiSubWindow*>& subWindows);

	QMdiArea* m_mdiArea;
	Ui::SubwindowsAlignDialog *ui;
};

#endif // SUBWINDOWSALIGNDIALOG_H
