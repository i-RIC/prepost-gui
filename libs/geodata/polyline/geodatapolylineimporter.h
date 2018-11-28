#ifndef GEODATAPOLYLINEIMPORTER_H
#define GEODATAPOLYLINEIMPORTER_H

#include "geodatapolylineimportersettingdialog.h"

#include <guicore/pre/geodata/geodataimporter.h>

#include <QVariant>

class GeoDataPolyLineImporter : public GeoDataImporter
{
	Q_OBJECT
public:
	GeoDataPolyLineImporter(GeoDataCreator* creator);
	virtual ~GeoDataPolyLineImporter();

	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(GeoData* data, int index, QWidget* w) override;

private:
	bool doInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w) override;

	GeoDataPolyLineImporterSettingDialog::NameSetting m_nameSetting;
	int m_nameAttribute;
	GeoDataPolyLineImporterSettingDialog::ValueSetting m_valueSetting;
	int m_valueAttribute;
	QVariant m_specifiedValue;

	int m_count;
};

#endif // GEODATAPOLYLINEIMPORTER_H
