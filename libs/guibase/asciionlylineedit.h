#ifndef ASCIIONLYLINEEDIT_H
#define ASCIIONLYLINEEDIT_H

#include "guibase_global.h"

#include <QLineEdit>

class GUIBASEDLL_EXPORT AsciiOnlyLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	AsciiOnlyLineEdit(QWidget* parent = 0);
	void clearIfNotAscii(bool withoutMessage = false);
	void setErrorMessage(const QString& msg);

public slots:
	void setText(const QString& text, bool warnIfNotAscii = false);

private:
	void focusOutEvent(QFocusEvent* e);

	QString m_errorMessage;
};

#endif // ASCIIONLYLINEEDIT_H
