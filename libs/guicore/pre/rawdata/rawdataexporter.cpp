#include "rawdatacreator.h"
#include "rawdataexporter.h"

RawDataExporter::RawDataExporter(RawDataCreator* creator)
	: QObject(creator)
{}

RawDataCreator* RawDataExporter::creator()
{
	return dynamic_cast<RawDataCreator*>(parent());
}
