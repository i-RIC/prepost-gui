#ifndef GEODATAPOINTMAPREALIMPORTER_H
#define GEODATAPOINTMAPREALIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPointmapRealImporter : public GeoDataImporter
{
	Q_OBJECT

private:
	enum filterString {
		dotTopo,      ///< Topography
		dotAdf,       ///< USGS NED(*.adf)
		dotStl        ///< STL (*.stl)
	};

public:
	GeoDataPointmapRealImporter(GeoDataCreator* creator);

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	int m_filterValue;
};

#endif // GEODATAPOINTMAPREALIMPORTER_H
