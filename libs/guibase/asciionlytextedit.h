#ifndef ASCIIONLYTEXTEDIT_H
#define ASCIIONLYTEXTEDIT_H

#include "guibase_global.h"
#include "textediti.h"

#include <QTextEdit>

class GUIBASEDLL_EXPORT AsciiOnlyTextEdit : public QTextEdit, public TextEditI
{
	Q_OBJECT

public:
	AsciiOnlyTextEdit(QWidget *parent = 0);
	void clearIfNotAscii(bool withoutMessage = false);
	void setErrorMessage (const QString& msg);

	QString text() const;

public slots:
	void setText(const QString& text);

private:
	void focusOutEvent(QFocusEvent* e);

	QString m_errorMessage;
};

#endif // ASCIIONLYTEXTEDIT_H
