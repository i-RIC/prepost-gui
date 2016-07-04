#include "iricrootpath.h"
#include <QString>

QString iRICRootPath::m_value;

QString iRICRootPath::get()
{
	return m_value;
}

void iRICRootPath::set(const QString& val)
{
	m_value = val;
}
