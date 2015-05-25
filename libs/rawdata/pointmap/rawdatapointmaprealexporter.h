#ifndef RAWDATAPOINTMAPREALEXPORTER_H
#define RAWDATAPOINTMAPREALEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataPointmapRealExporter : public RawDataExporter
{
	Q_OBJECT
public:
	RawDataPointmapRealExporter(RawDataCreator* creator);
	bool doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // RAWDATAPOINTMAPREALEXPORTER_H
