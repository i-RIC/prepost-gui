#ifndef PROJECTDATAITEM_IMPL_H
#define PROJECTDATAITEM_IMPL_H

#include "../projectdataitem.h"

#include <QString>

class ProjectDataItem::Impl
{
public:
	QString m_filename {};
	QString m_subPath {};
};

#endif // PROJECTDATAITEM_IMPL_H
