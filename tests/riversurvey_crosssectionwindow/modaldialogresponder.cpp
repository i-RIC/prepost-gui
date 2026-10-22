#include "modaldialogresponder.h"

#include <QAbstractButton>
#include <QApplication>
#include <QColorDialog>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

#include <iostream>

ModalDialogResponder::ModalDialogResponder(QObject* parent) :
	QObject {parent},
	m_lastDialog {nullptr}
{
	m_timer.setInterval(100);
	connect(&m_timer, &QTimer::timeout, this, &ModalDialogResponder::check);
	m_timer.start();
}

void ModalDialogResponder::setHandler(const Handler& handler)
{
	m_handler = handler;
}

void ModalDialogResponder::clearHandler()
{
	m_handler = Handler();
}

const QStringList& ModalDialogResponder::records() const
{
	return m_records;
}

void ModalDialogResponder::clearRecords()
{
	m_records.clear();
	m_lastDialog = nullptr;
}

void ModalDialogResponder::check()
{
	QWidget* w = QApplication::activeModalWidget();
	if (w == nullptr) {return;}
	if (! w->isVisible()) {return;}
	// progress dialogs close by themselves when the work finishes. closing them cancels the work.
	if (QString(w->metaObject()->className()) == "WaitDialog") {return;}

	QString text;
	auto mb = qobject_cast<QMessageBox*> (w);
	if (mb != nullptr) {
		text = mb->text();
	} else {
		text = w->windowTitle();
	}
	QString record = QString("%1: %2").arg(w->metaObject()->className()).arg(text);
	if (w != m_lastDialog) {
		m_records.push_back(record);
		std::cerr << "[ModalDialogResponder] " << record.toLocal8Bit().constData() << std::endl;
		m_lastDialog = w;
	}

	if (m_handler && m_handler(w)) {return;}
	respondDefault(w);
}

void ModalDialogResponder::respondDefault(QWidget* dialog)
{
	auto mb = qobject_cast<QMessageBox*> (dialog);
	if (mb != nullptr) {
		bool askSave = mb->text().contains("save", Qt::CaseInsensitive);
		QList<QMessageBox::StandardButton> order;
		if (askSave) {
			order << QMessageBox::No << QMessageBox::Discard << QMessageBox::Cancel;
		} else {
			order << QMessageBox::Yes << QMessageBox::Ok << QMessageBox::Close;
		}
		for (auto b : order) {
			auto button = mb->button(b);
			if (button != nullptr) {
				button->click();
				return;
			}
		}
		if (mb->defaultButton() != nullptr) {
			mb->defaultButton()->click();
			return;
		}
		mb->done(QMessageBox::Ok);
		return;
	}
	if (qobject_cast<QColorDialog*> (dialog) != nullptr || qobject_cast<QFileDialog*> (dialog) != nullptr) {
		qobject_cast<QDialog*> (dialog)->reject();
		return;
	}
	auto d = qobject_cast<QDialog*> (dialog);
	if (d != nullptr) {
		d->accept();
		return;
	}
	dialog->close();
}
