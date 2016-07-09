#include "lastiodirectory.h"

#include <QFileInfo>
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

void LastIODirectory::setFromFilename(const QString& filename)
{
	QFileInfo finfo(filename);

	set(finfo.absolutePath());
}
