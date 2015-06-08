#include "rawdatacreator.h"
#include "rawdataimporter.h"

RawDataImporter::RawDataImporter(RawDataCreator* creator)
	: QObject(creator)
{}

RawDataCreator* RawDataImporter::creator()
{
	return dynamic_cast<RawDataCreator*>(parent());
}

bool RawDataImporter::importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w)
{
	m_filename = filename;
	m_selectedFilter = selectedFilter;
	*count = 1;
	return doInit(filename, selectedFilter, count, condition, item, w);
}

bool RawDataImporter::doInit(const QString& /*filename*/, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorRawDataGroupDataItemInterface* /*item*/, QWidget* /*w*/)
{
	return true;
}
