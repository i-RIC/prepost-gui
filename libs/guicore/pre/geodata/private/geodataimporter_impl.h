#ifndef GEODATAIMPORTER_IMPL_H
#define GEODATAIMPORTER_IMPL_H

#include <QString>

#include <string>

#include "../geodataimporter.h"

class GeoDataImporter::Impl
{
public:
	std::string m_name;
	QString m_caption;

	QString m_filename;
	QString m_selectedFilter;
};

#endif // GEODATAIMPORTER_IMPL_H
