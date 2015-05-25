#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include "guibase_global.h"

#include <QDialog>

namespace Ui
{
	class WaitDialog;
}

/**
 * @brief Dialog to show "Please wait" message.
 *
 * It is possible to show progress information by progress bar.
 * It is also possible tho show "Cancel" button.
 *
 * It supports "unlimit mode". FOr example when waiting for
 * some process to finish, we can not know how long time it will take.
 * in such a case, we use unlimit mode. In umlimit mode, you can set
 * large values to setProgress().
 */
class GUIBASEDLL_EXPORT WaitDialog : public QDialog
{
	Q_OBJECT
public:
	explicit WaitDialog(QWidget* parent = nullptr);
	~WaitDialog();
	void setMessage(const QString& message);
	/// Show progress bar
	void showProgressBar();
	/// Disable cancel button
	void disableCancelButton();
	/// Set progress bar value range
	void setRange(int min, int max);
	/// Set the value of progress bar. It should be in the region set by setRange().
	void setProgress(int value);
	/// The value of progress bar
	int progress() const;
	/// Set the progress bar to the maximum value
	void setFinished();
	/// Set the dialog to "unlimit mode".
	void setUnknownLimitMode(int param);
	/// Handler for cancel button
	void reject() override;

signals:
	void canceled();

private:
	bool m_unknownLimitMode;
	unsigned int m_param;
	unsigned int m_progress;
	Ui::WaitDialog* ui;
};

#endif // WAITDIALOG_H
