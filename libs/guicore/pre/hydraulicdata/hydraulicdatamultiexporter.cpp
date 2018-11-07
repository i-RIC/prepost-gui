#include "hydraulicdata.h"
#include "hydraulicdatacreator.h"
#include "hydraulicdatamultiexporter.h"
#include "private/hydraulicdatamultiexporter_impl.h"

HydraulicDataMultiExporter::HydraulicDataMultiExporter(const QString& caption, HydraulicDataCreator* creator) :
	QObject {creator},
	impl {new Impl {}}
{
	impl->m_caption = caption;
}

HydraulicDataMultiExporter::~HydraulicDataMultiExporter()
{
	delete impl;
}

const QString& HydraulicDataMultiExporter::caption() const
{
	return impl->m_caption;
}

HydraulicDataCreator* HydraulicDataMultiExporter::creator() const
{
	return dynamic_cast<HydraulicDataCreator*>(parent());
}
