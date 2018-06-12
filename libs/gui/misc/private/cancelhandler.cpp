#include "cancelhandler.h"

CancelHandler::CancelHandler(QObject* parent) :
	QObject(parent),
	m_isCanceled {false}
{}

bool CancelHandler::isCanceled()
{
	return m_isCanceled;
}

void CancelHandler::cancel()
{
	m_isCanceled = true;
}
