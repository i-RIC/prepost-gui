#include "executerwatcher.h"
#include "executeri.h"

#include <QString>

ExecuterWatcher::ExecuterWatcher(ExecuterI* executer) :
	m_executer {executer}
{}

ExecuterWatcher::~ExecuterWatcher()
{
	delete m_executer;
}

void ExecuterWatcher::setMessage(const QString&)
{}

ExecuterI* ExecuterWatcher::executer() const
{
	return m_executer;
}
