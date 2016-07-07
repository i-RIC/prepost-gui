#ifndef ASCIIONLYLINEEDIT_H
#define ASCIIONLYLINEEDIT_H

#include "../guibase_global.h"
#include "textediti.h"

#include <QLineEdit>

class GUIBASEDLL_EXPORT AsciiOnlyLineEdit : public QLineEdit, public TextEditI
{
	Q_OBJECT

public:
	AsciiOnlyLineEdit(QWidget* parent = 0);
	void clearIfNotAscii(bool withoutMessage = false);
	void setErrorMessage(const QString& msg);

	QString text() const;

public slots:
	void setText(const QString& text) override;
	void setText(const QString& text, bool warnIfNotAscii);

private:
	void focusOutEvent(QFocusEvent* e);

	QString m_errorMessage;
};

#endif // ASCIIONLYLINEEDIT_H
