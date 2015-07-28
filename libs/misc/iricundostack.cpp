#include "iricundostack.h"

#include <QSettings>

const int iRICUndoStack::DEFAULT_UNDOLIMIT = 5;

iRICUndoStack* iRICUndoStack::m_instance = nullptr;

iRICUndoStack::iRICUndoStack(QObject* parent) :
	QUndoStack {parent}
{
	QSettings settings;
	int undoLimit = settings.value("general/undolimit", DEFAULT_UNDOLIMIT).toInt();
	setUndoLimit(undoLimit);
}

void iRICUndoStack::initialize(QObject* w)
{
	m_instance = new iRICUndoStack(w);
}

