#ifndef MOUSEHELPDIALOG_H
#define MOUSEHELPDIALOG_H

#include <QDialog>
#include <QPixmap>

namespace Ui
{
	class MouseHelpDialog;
}

class MouseHelpDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MouseHelpDialog(QWidget* parent = nullptr);
	~MouseHelpDialog();

protected:
	void closeEvent(QCloseEvent*) override;

private:
	Ui::MouseHelpDialog* ui;
	QPixmap m_image;
};

#endif // MOUSEHELPDIALOG_H
