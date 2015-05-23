#ifndef INFORMATIONDIALOG_H
#define INFORMATIONDIALOG_H

#include "misc_global.h"
#include <QDialog>

namespace Ui
{
	class InformationDialog;
}

/// Misc class to show message dialog with "Do not show this message again" checkbox
class MISCDLL_EXPORT InformationDialog : public QDialog
{
	Q_OBJECT

public:
	/// Show information dialog
	static void information(QWidget* parent, const QString& title, const QString& message, const QString& name);
	/// Show warning dialog
	static void warning(QWidget* parent, const QString& title, const QString& message, const QString& name);
	/// Show critical error dialog
	static void critical(QWidget* parent, const QString& title, const QString& message, const QString& name);

private:
	static void showDialog(QWidget* parent, const QString& title, const QString& message, const QString& name, const QPixmap& pixmap);
	explicit InformationDialog(QWidget* parent = nullptr);
	void setMessage(const QString& message);
	void setIcon(const QPixmap& pixmap);
	bool dontShowAgain();

public:
	~InformationDialog();

private:
	Ui::InformationDialog* ui;
};

#endif // INFORMATIONDIALOG_H
