#ifndef RAWDATARIVERSURVEYVTKEXPORTER_H
#define RAWDATARIVERSURVEYVTKEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataRiverSurveyVTKExporter : public RawDataExporter
{
	Q_OBJECT
public:
	RawDataRiverSurveyVTKExporter(RawDataCreator* creator);
	bool doEmport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd);
	const QStringList fileDialogFilters();
};

#endif // RAWDATARIVERSURVEYVTKEXPORTER_H
