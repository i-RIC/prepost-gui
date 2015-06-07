#ifndef RAWDATARIVERSURVEYVTKEXPORTER_H
#define RAWDATARIVERSURVEYVTKEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataRiverSurveyVTKExporter : public RawDataExporter
{
	Q_OBJECT

public:
	RawDataRiverSurveyVTKExporter(RawDataCreator* creator);
	bool doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // RAWDATARIVERSURVEYVTKEXPORTER_H
