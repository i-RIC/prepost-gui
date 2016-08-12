#ifndef CGNSFILEENTRY_IMPL_H
#define CGNSFILEENTRY_IMPL_H

#include "../cgnsfileentry.h"

#include <QString>

class CgnsFileEntry::Impl
{
public:
	Impl(const QString& filename);

	QString m_filename;
	QString m_comment;
};

#endif // CGNSFILEENTRY_IMPL_H
