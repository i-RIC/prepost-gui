#ifndef RAWDATAPOLYGONIMPORTER_H
#define RAWDATAPOLYGONIMPORTER_H

#include <guicore/pre/rawdata/rawdataimporter.h>
#include "rawdatapolygonimportersettingdialog.h"

class RawDataPolygonImporter : public RawDataImporter
{
	Q_OBJECT
public:
	RawDataPolygonImporter(RawDataCreator* creator) : RawDataImporter(creator) {}
	const QStringList fileDialogFilters() override;
	const QStringList acceptableExtensions() override;
	bool importData(RawData* data, int index, QWidget* w) override;

protected:
	bool doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, SolverDefinitionGridRelatedCondition* condition, PreProcessorRawDataGroupDataItemInterface* item, QWidget* w) override;

private:
	RawDataPolygonImporterSettingDialog::NameSetting m_nameSetting;
	int m_nameAttribute;
	RawDataPolygonImporterSettingDialog::ValueSetting m_valueSetting;
	int m_count;
	int m_valueAttribute;
	QVariant m_specifiedValue;
};

#endif // RAWDATAPOLYGONIMPORTER_H
