#ifndef GEODATAPOINTMAPREALTEXTIMPORTER_H
#define GEODATAPOINTMAPREALTEXTIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class CoordinateSystemConverter;

class GeoDataPointmapRealTextImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointmapRealTextImporter(GeoDataCreator* creator);
	~GeoDataPointmapRealTextImporter();

	bool importData(GeoData *data, int index, QWidget *w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private slots:
	void cancel();

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w) override;

	bool m_canceled;

	class LineParser;
	LineParser* m_parser;

	CoordinateSystemConverter* m_converter;

	class Values;
	class SettingDialog;
};

#endif // GEODATAPOINTMAPREALTEXTIMPORTER_H
