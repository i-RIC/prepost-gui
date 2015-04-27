#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include "guibase_global.h"

#include <QDialog>

namespace Ui {
	class WaitDialog;
}

class GUIBASEDLL_EXPORT WaitDialog : public QDialog
{
	Q_OBJECT
public:
	explicit WaitDialog(QWidget *parent = nullptr);
	~WaitDialog();
	void setMessage(const QString& message);
	void showProgressBar();
	void disableCancelButton();
	void setRange(int min, int max);
	void setProgress(int value);
	int progress() const;
	void setFinished();
	void setUnknownLimitMode(int param);
	void reject();

signals:
	void canceled();

private:
	bool m_unknownLimitMode;
	unsigned int m_param;
	unsigned int m_progress;
	Ui::WaitDialog *ui;
};

#endif // WAITDIALOG_H
