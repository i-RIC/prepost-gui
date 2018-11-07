#include "hydraulicdata.h"
#include "hydraulicdatacreator.h"
#include "hydraulicdataexporter.h"
#include "private/hydraulicdataexporter_impl.h"

HydraulicDataExporter::HydraulicDataExporter(const QString& caption, HydraulicDataCreator* creator) :
	QObject {creator},
	impl {new Impl {}}
{
	impl->m_caption = caption;
}

HydraulicDataExporter::~HydraulicDataExporter()
{
	delete impl;
}

const QString& HydraulicDataExporter::caption() const
{
	return impl->m_caption;
}

HydraulicDataCreator* HydraulicDataExporter::creator() const
{
	return dynamic_cast<HydraulicDataCreator*>(parent());
}
