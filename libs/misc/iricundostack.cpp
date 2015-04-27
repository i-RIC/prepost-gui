#include "iricundostack.h"

const int iRICUndoStack::UNDOLIMIT = 5;

iRICUndoStack* iRICUndoStack::m_instance = nullptr;

iRICUndoStack::iRICUndoStack(QObject *parent) :
	QUndoStack(parent)
{
	setUndoLimit(UNDOLIMIT);
}

void iRICUndoStack::initialize(QObject* w)
{
	m_instance = new iRICUndoStack(w);
}

