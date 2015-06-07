#ifndef RAWDATARIVERSURVEYEXPORTER_H
#define RAWDATARIVERSURVEYEXPORTER_H

#include <guicore/pre/rawdata/rawdataexporter.h>

class RawDataRiverSurveyExporter : public RawDataExporter
{
	Q_OBJECT

public:
	RawDataRiverSurveyExporter(RawDataCreator* creator);
	bool doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
};

#endif // RAWDATAPOINTMAPREALEXPORTER_H
