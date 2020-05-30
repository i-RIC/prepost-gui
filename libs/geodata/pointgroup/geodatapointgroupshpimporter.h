#ifndef GEODATAPOINTGROUPSHPIMPORTER_H
#define GEODATAPOINTGROUPSHPIMPORTER_H

#include <geodata/polydatagroup/geodatapolydatagroupshpimporter.h>

#include <QVariant>

class GeoDataPointGroupShpImporter : public GeoDataPolyDataGroupShpImporter
{
	Q_OBJECT

public:
	GeoDataPointGroupShpImporter(GeoDataCreator* creator);

	const QStringList fileDialogFilters() override;
	bool importData(GeoData* data, int index, QWidget* w) override;
};

#endif // GEODATAPOINTGROUPSHPIMPORTER_H
