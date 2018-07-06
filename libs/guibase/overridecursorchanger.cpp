#include "overridecursorchanger.h"

#include <QApplication>
#include <QCursor>

OverrideCursorChanger::OverrideCursorChanger(const QCursor& cursor)
{
    QApplication::setOverrideCursor(cursor);
    qApp->processEvents();
}

void OverrideCursorChanger::restoreCursor()
{
    QApplication::restoreOverrideCursor();
}

OverrideCursorChanger::~OverrideCursorChanger()
{
    restoreCursor();
}
