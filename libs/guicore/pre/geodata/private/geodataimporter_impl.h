#ifndef GEODATAIMPORTER_IMPL_H
#define GEODATAIMPORTER_IMPL_H

#include <QString>

#include <string>

#include "../geodataimporter.h"

class GeoDataImporter::Impl
{
public:
	Impl();
	~Impl();

	std::string m_name;
	QString m_caption;

	GeoDataImporterSetting* m_setting;
};

#endif // GEODATAIMPORTER_IMPL_H
