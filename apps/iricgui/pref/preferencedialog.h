#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
	class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreferenceDialog(QWidget *parent = 0);
	~PreferenceDialog();
	bool save();

public slots:
	void accept();

private:
	Ui::PreferenceDialog *ui;
};

#endif // PREFERENCEDIALOG_H
