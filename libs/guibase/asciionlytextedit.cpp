#include "asciionlytextedit.h"

#include <QMessageBox>
#include <QTextCodec>

AsciiOnlyTextEdit::AsciiOnlyTextEdit(QWidget *parent) :
	QTextEdit(parent)
{
	setAcceptRichText(false);
	m_errorMessage = tr("String has to consist of only English characters.");
}

void AsciiOnlyTextEdit::clearIfNotAscii(bool withoutMessage)
{
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	if (asciiCodec->canEncode(toPlainText())) {
		// no problem
		return;
	}
	if (withoutMessage) {return;}

	QMessageBox::warning(this, tr("Warning"), m_errorMessage);
}

void AsciiOnlyTextEdit::setErrorMessage (const QString& msg)
{
	m_errorMessage = msg;
}

QString AsciiOnlyTextEdit::text() const
{
	return QTextEdit::toPlainText();
}

void AsciiOnlyTextEdit::setText(const QString& text)
{
	QTextEdit::setPlainText(text);
	clearIfNotAscii(true);
}

void AsciiOnlyTextEdit::focusOutEvent(QFocusEvent* e)
{
	clearIfNotAscii();
	QTextEdit::focusOutEvent(e);
}
