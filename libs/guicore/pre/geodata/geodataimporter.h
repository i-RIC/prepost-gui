#ifndef GEODATAIMPORTER_H
#define GEODATAIMPORTER_H

#include "../../guicore_global.h"

#include <QObject>
#include <QString>

#include <string>

class GeoData;
class GeoDataCreator;
class GeoDataImporterSetting;
class SolverDefinitionGridAttribute;
class PreProcessorGeoDataDataItemI;
class PreProcessorGeoDataGroupDataItemI;

class GUICOREDLL_EXPORT GeoDataImporter : public QObject
{

public:
	GeoDataImporter(const std::string& name, const QString& caption, GeoDataCreator* creator);
	virtual ~GeoDataImporter();

	std::string name() const;
	QString caption() const;

	PreProcessorGeoDataDataItemI* import(const QString& filename, bool copyToProject, const QString& selectedFilter, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);
	PreProcessorGeoDataDataItemI* import(GeoDataImporterSetting* setting, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);

	GeoDataCreator* creator() const;

	virtual const QStringList fileDialogFilters() = 0;
	virtual const QStringList acceptableExtensions() = 0;

	bool importInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w, bool withSetting);
	virtual bool importData(GeoData* data, int index, QWidget* w) = 0;

	GeoDataImporterSetting* setting() const;

protected:
	virtual bool doInit(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);
	virtual bool doInitWithSetting(int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemI* item, QWidget* w);
	virtual GeoDataImporterSetting* createSetting() const;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/geodataimporter_impl.h"
#endif // _DEBUG

#endif // GEODATAIMPORTER_H
