#ifndef GEODATAPOINTMAPLANDXMLIMPORTER_H
#define GEODATAPOINTMAPLANDXMLIMPORTER_H

#include <guicore/pre/geodata/geodataimporter.h>

class GeoDataPointmapLandXmlImporter : public GeoDataImporter
{
public:
	GeoDataPointmapLandXmlImporter(GeoDataCreator* creator);

	bool importData(GeoData* data, int index, QWidget* w) override;
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
};

#endif // GEODATAPOINTMAPLANDXMLIMPORTER_H
