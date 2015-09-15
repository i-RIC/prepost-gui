#include "cursorchanger.h"
#include <QWidget>

CursorChanger::CursorChanger(const QCursor& cursor, QWidget* widget) :
	m_origCursor {widget->cursor()},
	m_widget {widget}
{
	m_widget->setCursor(cursor);
}

void CursorChanger::restoreCursor()
{
	if (m_widget == nullptr) {return;}
	m_widget->setCursor(m_origCursor);
	m_widget = nullptr;
}

CursorChanger::~CursorChanger()
{
	restoreCursor();
}
