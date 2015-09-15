#ifndef CURSORCHANGER_H
#define CURSORCHANGER_H

#include "guibase_global.h"
#include <QCursor>

class QWidget;

class GUIBASEDLL_EXPORT CursorChanger
{
public:
	CursorChanger(const QCursor& cursor, QWidget* widget);
	void restoreCursor();
	~CursorChanger();

private:
	CursorChanger(const CursorChanger& c) = delete;

	QCursor m_origCursor;
	QWidget* m_widget;
};

#endif // CURSORCHANGER_H
