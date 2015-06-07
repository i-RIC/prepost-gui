#include "lastiodirectory.h"

QString LastIODirectory::m_value;

const QString LastIODirectory::get()
{
	return m_value;
}

void LastIODirectory::set(const QString& val)
{
	m_value = val;
}
