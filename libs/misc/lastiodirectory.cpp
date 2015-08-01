#include "lastiodirectory.h"
#include <QString>

QString LastIODirectory::m_value;

QString LastIODirectory::get()
{
	return m_value;
}

void LastIODirectory::set(const QString& val)
{
	m_value = val;
}
