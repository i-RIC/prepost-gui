#include "ui_informationdialog.h"

#include "informationdialog.h"

#include <QMessageBox>
#include <QSettings>

InformationDialog::InformationDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::InformationDialog)
{
	ui->setupUi(this);
}

InformationDialog::~InformationDialog()
{
	delete ui;
}

void InformationDialog::information(QWidget* parent, const QString& title, const QString& message, const QString& name)
{
	QMessageBox mbox(parent);
	mbox.setIcon(QMessageBox::Information);
	QPixmap pixmap = mbox.iconPixmap();
	showDialog(parent, title, message, name, pixmap);
}

void InformationDialog::warning(QWidget* parent, const QString& title, const QString& message, const QString& name)
{
	QMessageBox mbox(parent);
	mbox.setIcon(QMessageBox::Warning);
	QPixmap pixmap = mbox.iconPixmap();
	showDialog(parent, title, message, name, pixmap);
}

void InformationDialog::critical(QWidget* parent, const QString& title, const QString& message, const QString& name)
{
	QMessageBox mbox(parent);
	mbox.setIcon(QMessageBox::Critical);
	QPixmap pixmap = mbox.iconPixmap();
	showDialog(parent, title, message, name, pixmap);
}

void InformationDialog::showDialog(QWidget* parent, const QString& title, const QString& message, const QString& name, const QPixmap& pixmap)
{
	QSettings settings;
	QString fullname = QString("infodialog/%1").arg(name);
	bool dontshowagain = settings.value(fullname, false).value<bool>();
	if (dontshowagain) {
		// do not show!
		return;
	}
	InformationDialog dialog(parent);
	dialog.setWindowTitle(title);
	dialog.setMessage(message);
	dialog.setIcon(pixmap);
	dialog.resize(450, dialog.minimumHeight());
	dialog.exec();
	dontshowagain = dialog.dontShowAgain();
	settings.setValue(fullname, dontshowagain);
}

void InformationDialog::setMessage(const QString& message)
{
	ui->messageLabel->setText(message);
}

void InformationDialog::setIcon(const QPixmap& pixmap)
{
	ui->iconLabel->setPixmap(pixmap);
}

bool InformationDialog::dontShowAgain()
{
	return ui->checkBox->isChecked();
}
