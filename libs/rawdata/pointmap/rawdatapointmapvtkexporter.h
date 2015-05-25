#ifndef RAWDATAPOINTMAPVTKEXPORTER_H
#define RAWDATAPOINTMAPVTKEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataPointmapVTKExporter : public RawDataExporter
{
	Q_OBJECT
public:
	RawDataPointmapVTKExporter(RawDataCreator* creator);
	bool doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // RAWDATAPOINTMAPVTKEXPORTER_H
