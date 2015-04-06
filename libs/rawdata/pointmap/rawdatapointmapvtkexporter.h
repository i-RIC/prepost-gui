#ifndef RAWDATAPOINTMAPVTKEXPORTER_H
#define RAWDATAPOINTMAPVTKEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataPointmapVTKExporter : public RawDataExporter
{
	Q_OBJECT
public:
	RawDataPointmapVTKExporter(RawDataCreator* creator);
	bool doEmport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd);
	const QStringList fileDialogFilters();
};

#endif // RAWDATAPOINTMAPVTKEXPORTER_H
