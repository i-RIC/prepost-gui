#ifndef GEODATAPOINTGROUPSHPIMPORTER_H
#define GEODATAPOINTGROUPSHPIMPORTER_H

#include "geodatapointgroupshpimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

class GeoDataPointGroupShpImporter : public GeoDataImporter
{
	Q_OBJECT

public:
	GeoDataPointGroupShpImporter(GeoDataCreator* creator);
	~GeoDataPointGroupShpImporter() override;

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	GeoDataPointGroupShpImporterSettingDialog::NameSetting m_nameSetting;
	int m_nameAttribute;
	GeoDataPointGroupShpImporterSettingDialog::ValueSetting m_valueSetting;
	int m_count;
	int m_valueAttribute;
	QVariant m_specifiedValue;
};

#endif // GEODATAPOINTGROUPSHPIMPORTER_H
