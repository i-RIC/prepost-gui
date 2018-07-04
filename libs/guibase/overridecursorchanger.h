#ifndef OVERRIDECURSORCHANGER_H
#define OVERRIDECURSORCHANGER_H

#include "guibase_global.h"

class QCursor;

class GUIBASEDLL_EXPORT OverrideCursorChanger
{
public:
    OverrideCursorChanger::OverrideCursorChanger(const QCursor& cursor);

    OverrideCursorChanger::~OverrideCursorChanger();

    void OverrideCursorChanger::restoreCursor();
};

#endif // OVERRIDECURSORCHANGER_H
